package ptree

import (
	"testing"
)

func TestPtreeAdd(t *testing.T) {
	ptree := NewPtree()

	ptree.InsertPath("aaa:1111", 3)
	ptree.InsertPath("aaa:1111:bbb", 3)

	if len(ptree.LevelRoot) != 3 {
		t.Errorf("Got %v expected %v", len(ptree.LevelRoot), 3)
	}
}

func TestPtreeAddIncremental(t *testing.T) {
	ptree := NewPtree()

	ptree.InsertPath("aaa", 3)
	if len(ptree.LevelRoot) != 1 {
		t.Errorf("Got %v expected %v", len(ptree.LevelRoot), 1)
	}

	ptree.InsertPath("aaa:1111", 3)
	if len(ptree.LevelRoot) != 2 {
		t.Errorf("Got %v expected %v", len(ptree.LevelRoot), 2)
	}
	ptree.InsertPath("aaa:1111:bbb", 3)
	if len(ptree.LevelRoot) != 3 {
		t.Errorf("Got %v expected %v", len(ptree.LevelRoot), 3)
	}

	ptree.InsertPath("aaa:1111:bbb:222", 3)
	if len(ptree.LevelRoot) != 4 {
		t.Errorf("Got %v expected %v", len(ptree.LevelRoot), 4)
	}
}
