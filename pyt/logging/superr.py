import logging 
class someClass():
    def __init__(self, loggerName="root"):
        self.logger = logging.getLogger(loggerName)
        self.logger.info("super clas.")
        self.someVar = 0
    
    def funcA(self):
        self.someVar += 1
    
    def funcB(self):
        self.someVar += 1

    def __del__(self):
        self.logger.info("someClass is being deconstructed.")