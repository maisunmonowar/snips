from superr import someClass
import logging 

class myChile(someClass):
    def __init__(self):
        self.logger = logging.getLogger("ektanaam")
        self.logger.info("child.")
        super().__init__()
        self.myVar = 5

    def funcB(self):
        self.someVar += self.myVar
        return self.someVar
    
    def getSomeVar(self):
        return self.someVar