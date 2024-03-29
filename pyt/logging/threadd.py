import time, logging

class someThreadThing():
    def __init__(self):
        self.logger = logging.getLogger("ektanaam")
        self.logger.info("thread init")
        self._running = True
    
    def terminate(self):
        self._running = False

    def run(self):
        counter = 0
        while self._running and counter < 10:
            
            self.logger.info("thread running")
            time.sleep(1)
            counter += 1
        self.terminate()