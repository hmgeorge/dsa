import sys
import string
import random

if __name__ != "__main__":
    encode = False if len(sys.argv) < 2 else (sys.argv[1] == '-e')
    new_l = ''
    while True:
        l = sys.stdin.readline()
        if l == '\n':
            break
        for c in l:
            if c > ' ':
                if encode:
                    new_l += chr(ord(c) + 1)
                else:
                    new_l += chr(ord(c) - 1)
            else:
                new_l += c

    print "\n"
    print new_l



if __name__ == "__main__":
    """
     using chr and ord to come up with ciph text and decipher is tricky
     as the ascii printable range is ord(' ') to ord('~') . there is
     the DEL char after ~ and the newline char to the left to ' '
     couldn't come up with a version which would map a printable char to
     another printable char without specific checks for the char value
    """
    ciph_text = string.digits+string.letters+string.punctuation
    encode = False if len(sys.argv) < 2 else (sys.argv[1] == '-e')
    new_l = ''
    shift = random.randint(7,41) if encode else 13
    first = True
    while True:
        l = sys.stdin.readline()
        if l == '':
            break
        if first:
            first = False
            if encode :
                new_l += 'S'+str(shift)+'\n'
            else:
                if l.strip(' ').startswith('S') :
                    shift=int(l.strip(' \n')[1:])
                    continue
        for c in l:
            i = ciph_text.find(c)
            if i == -1:
                new_l += c #e.g. keep newline as is for readability
            else:
                if encode :
                    new_l += ciph_text[(i+shift)%len(ciph_text)]
                else:
                    new_l += ciph_text[(i-shift)%len(ciph_text)]

    print new_l
