#include "bbProcess.h"

void bbProcessor::stripBB(std::queue<custom_struct_for_queue> &q_dataToProcess,
                          std::mutex &mut_main2support,
                          std::condition_variable &cv_main_to_bb,
                          std::atomic<bool> &ab_should_worker_be_working) {

  bool somethingWaitingToBeProcessed = false;
  char *buf_bbFrame = new char[c_BufSize];
  char *buf_extractedData = new char[c_BufSize * 2];
  // todo: Unnecessary heap allocation for simple integers.
  int *buf_extractedData_len = new int;
  int buf_bbFrame_len = c_BufSize;
  *buf_extractedData_len = c_BufSize * 2;
  
  // queue and other things for another thread
  std::queue<custom_struct_for_queue> queue_bbFrame_tf;
  std::condition_variable cv_bbFrame_tf;
  std::mutex mutex_bbFrame_tf;
  std::atomic<bool> kill_signal_bb_to_tf(false);

  
  auto tf_processor = tfProcessor();
  
  std::thread worker_tf_processor =
      std::thread(&tfProcessor::sanitizeTf, &tf_processor,
                  std::ref(queue_bbFrame_tf), std::ref(mutex_bbFrame_tf),
                  std::ref(cv_bbFrame_tf), std::ref(kill_signal_bb_to_tf));

  while (true) {
    {
      std::unique_lock<std::mutex> lock{mut_main2support};
      cv_main_to_bb.wait(lock, [&] {
        return !q_dataToProcess.empty() || !ab_should_worker_be_working.load();
      });

      if (!q_dataToProcess.empty()) {
        memcpy(buf_bbFrame, q_dataToProcess.front().pointerToByte,
               q_dataToProcess.front().numOfByte);
        buf_bbFrame_len = q_dataToProcess.front().numOfByte;
        std::cout << "BB Process says: " << buf_bbFrame << std::endl;
        somethingWaitingToBeProcessed = true;
        delete q_dataToProcess.front().pointerToByte;
        q_dataToProcess.pop();
      } // if !data.empty()
    }   // mutex is out of scope
        
    
    // time to process the data
    if (somethingWaitingToBeProcessed) {
      // pretend the buf bbframe is the cleaned up data
      memcpy(buf_extractedData, buf_bbFrame, buf_bbFrame_len); 
      somethingWaitingToBeProcessed =          false; // because at this point every thing is processed.
      {
        std::lock_guard<std::mutex> lock{mutex_bbFrame_tf};
        queue_bbFrame_tf.emplace(new char[*buf_extractedData_len],
                                 *buf_extractedData_len);
        // memcopy
        memcpy(queue_bbFrame_tf.back().pointerToByte, buf_extractedData,
               *buf_extractedData_len);
        cv_bbFrame_tf.notify_all();
      }
    }
    if (ab_should_worker_be_working.load()) {
      // pass
    } else {
      
      break;
    }
  } // end of infinited loop

  kill_signal_bb_to_tf.store(true);
  cv_bbFrame_tf.notify_all();
  worker_tf_processor.join();
  delete[] buf_bbFrame;
  delete[] buf_extractedData;
  delete buf_extractedData_len;
}