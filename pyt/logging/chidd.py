from superr import someClass
import logging 

class myChile(someClass):
    def __init__(self, loggerName="root"):
        self.logger = logging.getLogger(loggerName)
        self.logger.info("child.")
        super().__init__()
        self.myVar = 5

    def funcB(self):
        self.someVar += self.myVar
        return self.someVar
    
    def getSomeVar(self):
        return self.someVar
    
    def __del__(self):
        self.logger.info("myChile is being deconstructed.")