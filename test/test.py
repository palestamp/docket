import subprocess
import difflib
import sys
import unittest
import os
import shutil

root_path = lambda x: os.path.join("../../", x)
guage_folder = lambda x: root_path(os.path.join("test/expected", x))

def rm_r(path):
    if os.path.isdir(path) and not os.path.islink(path):
        shutil.rmtree(path)
    elif os.path.exists(path):
        os.remove(path)

class DocketBin(object):
    _binary = root_path("build/src/docket")

    def send(self, command):
        cmd = "{} {}".format(self._binary, command)
        out = subprocess.check_output(cmd, shell=True)
        return out


def generate_diff(expected, real):
    for line in difflib.context_diff(expected.split('\n'), real.split('\n')):
        sys.stdout.write(line)


class DocketTest(unittest.TestCase):
    def setUp(self):
        self.docket = DocketBin()
        self.dir = ".test"
        os.mkdir(self.dir)
        self.p = lambda path: os.path.join(self.dir, path)

    def _add_source(self, name, source_path):
        self.docket.send("add --name {} --use-config {} {}".format(name, self.p(".docket"), source_path))

    def _create_source(self, fname, content):
        with open(fname, "w+") as source:
            if isinstance(content, str):
                source.write(content)

    def tearDown(self):
        rm_r(self.dir)


class TestConfig(DocketTest):
    def test_add_cmd_long(self):
        self._create_source(self.p("test.docket"), None)
        self.docket.send("add --name {} --use-config {} {}".format("test", self.p(".docket"), self.p("test.docket")))

        with open(guage_folder("add.out")) as ef:
            expected = ef.read()
            expected = expected.replace("TEST_SOURCE", os.path.abspath(self.p("test.docket")))

        with open(self.p(".docket")) as gf:
            got = gf.read()

        self.assertEqual(expected, got)

    def test_add_cmd_short(self):
        self._create_source(self.p("test.docket"), None)
        self.docket.send("add -n {} -c {} {}".format("test", self.p(".docket"), self.p("test.docket")))

        with open(guage_folder("add.out")) as ef:
            expected = ef.read()
            expected = expected.replace("TEST_SOURCE", os.path.abspath(self.p("test.docket")))

        with open(self.p(".docket")) as gf:
            got = gf.read()

        self.assertEqual(expected, got)


if __name__ == '__main__':
    unittest.main(verbosity=2)
