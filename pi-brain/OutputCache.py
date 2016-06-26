##

import array

class OutputCache:
    def __init__(self):
        self.count = 0
        self.queue = array.array('B')

    def add(self, b):
        self.count += 1
        self.queue.append(b)
        if self.count > 2048:
            self.dump()
            self.count = 0
            self.queue = array.array('B')

    def dump(self):
        n = 0
        mesg = ""
        mesg2 = ""
        for i in xrange(len(self.queue)):
            mesg += format(self.queue[i], '#04x') + " "
            mesg2 += format(self.queue[i], '03d') + " "
            n += 1
            if n % 8 == 0:
                mesg += " "
                mesg2 += " "
            if n >= 16:
                print mesg + "  " + mesg2
                mesg = ""
                mesg2 = ""
                n = 0

