import subprocess
import configparser
import sys

def testassert(expected,value,expression):
    if value != expected: 
        print(f"TEST FAILED: Expected {expected} not {value}. Expression: {expression}")
        return 1
    return 0

if (len(sys.argv) != 2):exit(1)
file = sys.argv[1]

parsed = configparser.ConfigParser()
parsed.read(file)

#print(dict(parsed))

failcount = 0
testcount = 0
sectioncount = 0
tot = len(parsed.sections())
for sect in parsed.sections():
    sectioncount+=1
    print(f"Checking section {sectioncount}/{tot}")
    for item in parsed.items(sect):
        ret = subprocess.run(args=[f"./ini-parser {file} {sect}.{item[0]}"],capture_output=True,shell=True,text=True).stdout.strip()
        failcount += testassert(item[1],ret,f"{sect}.{item[0]}")
        testcount += 1
        for item2 in parsed.items(sect):
            for action in "+-/*":
                testcount += 1
                ret = subprocess.run(args=[f'./ini-parser {file} "{sect}.{item[0]} {action} {sect}.{item2[0]}"'],capture_output=True,shell=True,text=True).stdout.strip()
                expect = "Cannot perform operations on mixed data types."
                if item[1].isnumeric() and item2[1].isnumeric():
                    match action:
                        case "+":
                            expect = int(item[1])+int(item2[1])
                            break
                        case "-":
                            expect = int(item[1])-int(item2[1])
                            break
                        case "/":
                            expect = int(item[1])//int(item2[1])
                            break
                        case "*":
                            expect = int(item[1])*int(item2[1])
                            break
                elif not (item[1].isnumeric() or item2[1].isnumeric()):
                    if action == '+':
                        expect = item[1] + item2[1]
                    else: expect = f"Unsupported operand '{action}' on strings."
                

                
                failcount += testassert(str(expect),ret,f"{sect}.{item[0]} {action} {sect}.{item2[0]}")

print(f"Tests passed: {testcount-failcount}/{testcount}\nTests failed: {failcount}/{testcount}")