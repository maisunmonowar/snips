import logging 
class someClass():
    def __init__(self):
        self.logger = logging.getLogger("ektanaam")
        self.logger.info("super clas.")
        self.someVar = 0
    
    def funcA(self):
        self.someVar += 1
    
    def funcB(self):
        self.someVar += 1
