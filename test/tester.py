import difflib
import os
import sys
import re

# ////////// config
# 自定义程序路径
exe_path = "..\\src\\cmake-build-debug\\leoml.exe"

ANSI_COLOR_BLUE = "\x1b[34m"
ANSI_COLOR_RESET = "\x1b[0m"

# \\\\\\\\\\


class Tester:
    def __init__(self, number: int):
        super().__init__()
        self.lexer = exe_path + ' -l'
        self.parser = exe_path + ' -p'
        self.number = number

    def test_lexer(self) -> bool:
        result = ''
        with open(f'./ts/{self.number}.ts.txt') as f:
            tts = f.read()
        try:
            result = os.popen(self.lexer).read()
        except TimeoutError:
            print('execute error')
        diff = list(difflib.context_diff(result, tts))
        if len(diff) == 0:
            return True
        else:
            [print(line) for line in diff]
            return False

    def test_parser(self) -> bool:
        result = ''
        with open(f'./ast/{self.number}.ast.txt') as f:
            tts = f.read()
        try:
            result = os.popen(self.parser).read()
        except TimeoutError:
            print('execute error')
        diff = list(difflib.context_diff(result, tts))
        if len(diff) == 0:
            return True
        else:
            [print(line) for line in diff]
            return False


def print_with_color(start, msg):
    print(ANSI_COLOR_BLUE+start+ANSI_COLOR_RESET+": \n"+msg)


class TesterCausal:
    def __init__(self):
        super().__init__()
        self.lexer = exe_path + ' -l '
        self.parser = exe_path + ' -p '

    def test_parser(self, filename: str):
        print_with_color('='*20, '')
        with open(filename, 'r') as f:
            title = f.readline()[:-1].strip("(*) ")
            content = f.read()
            print_with_color("testcase title", title)
            print_with_color("testcase content", content)
        result = None
        try:
            out = os.popen(self.parser+filename)
            out.readline()
            result = out.read()
        except:
            print("execute error")
        print_with_color("parse result", result)


# not importent
def gen_txt():
    for i in range(10, 20):
        with open(f'./ml/{i}.ml.txt', 'w') as f:
            f.write('')
        with open(f'./ts/{i}.ts.txt', 'w') as f:
            f.write('')
        with open(f'./ast/{i}.ast.txt', 'w') as f:
            f.write('')


def main():
    tester = TesterCausal()
    for i in [0, 1, 2] + [i for i in range(10, 17)]:  # positive + negative
        tester.test_parser("./ml/%.2d.ml.txt" % (i))


if __name__ == '__main__':
    main()
