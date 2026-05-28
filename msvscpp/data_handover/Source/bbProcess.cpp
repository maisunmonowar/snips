#include "bbProcess.h"

void bbProcessor::stripBB(std::queue<DataChunk> &q_dataToProcess,
                          std::mutex &mut_main2support,
                          std::condition_variable &cv_main_to_bb,
                          std::atomic<bool> &ab_should_worker_be_working) {

  bool somethingWaitingToBeProcessed = false;
  
  // queue and other things for another thread
  std::queue<DataChunk> queue_bbFrame_tf;
  std::condition_variable cv_bbFrame_tf;
  std::mutex mutex_bbFrame_tf;
  std::atomic<bool> kill_signal_bb_to_tf(false);

  
  auto tf_processor = tfProcessor();
  
  std::thread worker_tf_processor =
      std::thread(&tfProcessor::sanitizeTf, &tf_processor,
                  std::ref(queue_bbFrame_tf), std::ref(mutex_bbFrame_tf),
                  std::ref(cv_bbFrame_tf), std::ref(kill_signal_bb_to_tf));

  while (true) {
    DataChunk localChunk;
    {

      std::unique_lock<std::mutex> lock{mut_main2support};
      cv_main_to_bb.wait(lock, [&] {
        return !q_dataToProcess.empty() || !ab_should_worker_be_working.load();
      });

      if (!q_dataToProcess.empty()) {
        localChunk = std::move(q_dataToProcess.front());
        q_dataToProcess.pop();
        std::cout << "BB Process says: ";
        std::cout.write(localChunk.data(), localChunk.size());
        std::cout << std::endl;
        somethingWaitingToBeProcessed = true;
      } // if !data.empty()
    }   // mutex is out of scope
        
    
    // time to process the data
    if (somethingWaitingToBeProcessed) {
      // pretend the localChunk is the cleaned up data
      //memcpy(buf_extractedData, buf_bbFrame, buf_bbFrame_len); >> this is no longer needed.
      somethingWaitingToBeProcessed =          false; // because at this point every thing is processed.
      // note: as we are storing this data in RAM there is no rush to process this data. 
      // a little bit of delay is acceptable.
      {
        std::lock_guard<std::mutex> lock{mutex_bbFrame_tf};
        queue_bbFrame_tf.emplace(std::move(localChunk));
        cv_bbFrame_tf.notify_all();
      }
    }
    // Only exit the loop if we're told to stop AND we have fully emptied the
    // queue
    if (!ab_should_worker_be_working.load() && q_dataToProcess.empty()) {
      break;
    }
  } // end of infinited loop

{
    std::lock_guard<std::mutex> lock{mutex_bbFrame_tf};
    kill_signal_bb_to_tf.store(true);
    cv_bbFrame_tf.notify_all(); // Safe!
  }
  worker_tf_processor.join();
}