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

    thisLoggerName = "ektanaam"
    logger = logging.getLogger("ektanaam")
    logger.setLevel(logging.DEBUG)

    handler = TimedRotatingFileHandler('logs/snip.log', when='midnight', backupCount=5)
    handler.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(filename)s:%(lineno)d - %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)

    # Adding a StreamHandler to the logger
    stream_handler = logging.StreamHandler()
    stream_handler.setLevel(logging.DEBUG)
    stream_handler.setFormatter(formatter)
    logger.addHandler(stream_handler)
    
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


    obj = chClass(loggerName=thisLoggerName)
    obj.getSomeVar()
    
    # if threadObj_thread.is_alive():
        
    #     threadObj_thread.join()
    # threadObj = thClass(loggerName=thisLoggerName)
    # threadObj_thread = threading.Thread(target=threadObj.run)
    # threadObj_thread.start()

    logger.info("Shutting down")

    import datetime
    habba = 100000
    starttime = (datetime.datetime.now())
    for _ in range(habba):
        logger.debug("asldfjalskfdja;lskfj;alskfdj;laskjfdalksdjfl;asdfjl;asfalkflasdjflasjdflakdfjlaskjdflaksdjflakjsdflkasjdlfkjasdlfjalsfjalsfjasl;fjalsfkj")
    breaktime = (datetime.datetime.now())
    for _ in range(habba):
        print("asldfjalskfdja;lskfj;alskfdj;laskjfdalksdjfl;asdfjl;asfalkflasdjflasjdflakdfjlaskjdflaksdjflakjsdflkasjdlfkjasdlfjalsfjalsfjasl;fjalsfkj")
    endtime = (datetime.datetime.now())

    print("debug took")
    print(breaktime - starttime)
    print()
    print("print took")
    print(endtime-breaktime)
    exit(0)
