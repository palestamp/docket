package ptree

import (
	"strings"
)

const (
	PATH_DELIMITER = ":"
)

// General Tree --------------------------------------------------------------------

// Top - Down search And common structure layout (ex: "a.b.*")
type GeneralNode struct {
	parent   *GeneralNode
	Label    string         "l"
	Children []*GeneralNode "c,omitempty"
	Data     int            // this should be array of vals
}

func NewGeneralNode(label string) *GeneralNode {
	return &GeneralNode{Label: label, Children: make([]*GeneralNode, 0)}
}

func (self *GeneralNode) AddChild(label string) *GeneralNode {
	for _, child := range self.Children {
		if label == child.Label {
			return child
		}
	}
	c := NewGeneralNode(label)
	c.parent = self
	self.Children = append(self.Children, c)
	return c
}

func (self *GeneralNode) InsertPath(path string, data int) *GeneralNode {
	labels := strings.Split(path, PATH_DELIMITER)
	lc := len(labels)
	i := 0

	swap := self
	for ; i < lc-1; i++ {
		swap = swap.AddChild(labels[i])
	}
	swap = swap.AddChild(labels[i])
	swap.Data = data
	return swap
}

func (self *GeneralNode) FindPath(path string) *GeneralNode {
	labels := strings.Split(path, PATH_DELIMITER)
	swap := self
	for _, label := range labels {
		for _, node := range swap.Children {
			if node != nil {
				if node.Label == label {
					swap = node
				}
			} else {
				return nil
			}
		}
	}
	return swap
}

// Shadow Index (Level) ------------------------------------------------------------

// UniqLevelNode struct intendet to hold uniq label within one horizontal
// level of Ptree. Collecting pointer to GeneralNodes splitted in groups
// by theirs parent id. This give possibility to recursively track identifier
// of full path from each GeneralNode group to the Ptree root
// In general I think that probability of occurance of great node number in one
// UniqLevelNode is pretty low - so we can use some kind of binary tree

type UniqLevelNode struct {
	label string
	m     map[*GeneralNode][]*GeneralNode
}

func NewUniqLevelNode(label string) *UniqLevelNode {
	return &UniqLevelNode{label: label,
		m: make(map[*GeneralNode][]*GeneralNode)}
}

func (self *UniqLevelNode) InsertGeneralNode(gn *GeneralNode) {
	if gn == nil {
		return
	}
	key := gn.parent
	if _, ok := self.m[key]; ok {
		self.m[key] = append(self.m[key], gn)
	} else {
		self.m[key] = make([]*GeneralNode, 0)
		self.m[key] = append(self.m[key], gn)
	}

}

// Collection of UniqLevelNodes.
// Level.items surely should be BST or Hash Table instead of slice.

type Level struct {
	Depth int
	items []*UniqLevelNode
}

func NewLevel(depth int) *Level {
	return &Level{Depth: depth, items: make([]*UniqLevelNode, 0)}
}

func (self *Level) InsertGeneralNode(label string, gn *GeneralNode) {
	for _, my := range self.items {
		if my.label == label {
			my.InsertGeneralNode(gn)
			return
		}
	}

	newULN := NewUniqLevelNode(label)
	newULN.InsertGeneralNode(gn)
	self.items = append(self.items, newULN)
}

// Ptree ---------------------------------------------------------------------------

// Interface exposed data structure
type Ptree struct {
	GeneralRoot *GeneralNode
	LevelRoot   []*Level "-"
}

func NewPtree() *Ptree {
	return &Ptree{GeneralRoot: NewGeneralNode(""),
		LevelRoot: make([]*Level, 0)}
}

// Return Level of LevelRoot. If Level not exists - create it
func (self *Ptree) GetLevel(level int) *Level {
	if len(self.LevelRoot) <= level {
		self.LevelRoot = append(self.LevelRoot, NewLevel(level))
	}
	return self.LevelRoot[level]
}

func (self *Ptree) InsertPath(path string, data int) {
	gn := self.GeneralRoot.InsertPath(path, data)

	labels := strings.Split(path, PATH_DELIMITER)
	lc := len(labels)
	i := 0
	for ; i < lc-1; i++ {
		level := self.GetLevel(i)
		level.InsertGeneralNode(labels[i], nil)
	}
	level := self.GetLevel(i)
	level.InsertGeneralNode(labels[i], gn)
}

func (self *Ptree) GetNode(path string) *GeneralNode {
	return self.GeneralRoot.FindPath(path)
}
