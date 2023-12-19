import threading, time, logging, argparse, os
from chidd import myChile as chClass
from threadd import someThreadThing as thClass
from logging.handlers import TimedRotatingFileHandler

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-v", "--verbose", help="increase output verbosity", action="store_true")
    parser.add_argument("-d", "--debug", help="increase output verbosity", action="store_true")
    args = parser.parse_args()

    if not os.path.exists("logs"):
        os.makedirs("logs")

    logger = logging.getLogger("ektanaam")
    logger.setLevel(logging.DEBUG)

    handler = TimedRotatingFileHandler('logs/snip.log', when='midnight', backupCount=5)
    handler.setLevel(logging.DEBUG)
    # formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(filename)s:%(lineno)d - %(funcName)s - %(message)s - %(module)s - %(created)f - %(relativeCreated)d - %(msecs)d - %(exc_info)s - %(exc_text)s - %(stack_info)s - %(pathname)s')
    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(filename)s:%(lineno)d - %(message)s')
    
    '''
    Notes:
    module - mostly the class name. 
    funcName - self explanatory
    '''
    handler.setFormatter(formatter)
    logger.addHandler(handler)

    # if verbose is set, add another streamHandler to logger
    if args.verbose:
        streamHandler = logging.StreamHandler()
        streamHandler.setLevel(logging.DEBUG)
        streamHandler.setFormatter(formatter)
        logger.addHandler(streamHandler)
    

    logger.info("Starting up")

    threadObj = thClass()
    threadObj_thread = threading.Thread(target=threadObj.run)
    threadObj_thread.start()

    obj = chClass()
    obj.getSomeVar()
    
    if threadObj_thread.is_alive():
        
        threadObj_thread.join()

    logger.info("Shutting down")
