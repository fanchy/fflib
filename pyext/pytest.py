import channle
def test(ptr, msg):
    print(ptr, msg)
    print("in python...", channle.send(111, 'echo "ok===="'))
    return 101

print('load end ok .......')
