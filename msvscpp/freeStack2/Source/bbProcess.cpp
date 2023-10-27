#include "bbProcess.h"
#include "tfProcess.h"

int bbProcessor::substract(int a, int b) { return a - b; }
void bbProcessor::work(
	char* buffer_withHeader,
	int* buffer_withHeader_len,
	char* buffer_withoutHeader,
	int* buffer_withoutHeader_len,
	bool enableLogging = true)
{
	/* in the incoming buffer, we need to
	process frame by frame
	we need a int to point which frame we are
	currently working with.
	also an end.*/
	uint16_t datafield_length;
	uint8_t bbFrameID;
	int startOfCurrentFrame, startOfNextFrame;
	bool isMarkerFound = false;
	startOfCurrentFrame = 0;
	*buffer_withoutHeader_len = 0; // haven't done anything yet. so zero. 
	if ((unsigned char)buffer_withHeader[0] == 0xb8)
	{
		isMarkerFound = true;
		startOfCurrentFrame = 0;
		//	std::cout << "\t\t Marker found. " << std::endl;
	}
	else
	{
		for (int i = 0; i < *buffer_withHeader_len; i++)
		{
			if ((unsigned char)buffer_withHeader[i] == 0xb8)
			{
				startOfCurrentFrame = i;
				isMarkerFound = true;
				break;
			}
		}
		if (!isMarkerFound)
		{
			std::cout << "BB Frame Sync Marker not present. todo";

			/* if the marker is later in the buffer,
			then perhaps it contains the data
			we were waiting for*/
			if (buffer_incompleteData_len > 0)
			{
				// looks like there is unprocessed data in our buffer
				memcpy(buffer_incompleteData + buffer_incompleteData_len, buffer_withHeader, *buffer_withHeader_len);
				buffer_incompleteData_len += *buffer_withHeader_len;
				// so our buffer_incomplete is now infact complete
				bbFrameID = (uint8_t)buffer_incompleteData[3];
				datafield_length = buffer_incompleteData[8];
				datafield_length = (datafield_length << 8) & 0xff00;
				datafield_length += ((uint16_t)buffer_incompleteData[9]) & 0x00ff;
				datafield_length = datafield_length / 8;

				// time to push it to the outgoing buffer
				memcpy(buffer_withoutHeader + *buffer_withoutHeader_len, buffer_incompleteData + 14, datafield_length);
				*buffer_withoutHeader_len = datafield_length;


				// our temp buffer now ready to receive more data
				buffer_incompleteData_len = 0;
				// ready for the next frame

			}
			//else
			//{
			// todo
			//the marker was later in file. but we have nothing in buffer. so just work with it. 
			// the varialbles are already set. need to test this further. 
			//}
		}
	}

	//loop start
	while (startOfCurrentFrame < *buffer_withHeader_len && isMarkerFound)
	{
		// before anything we need to check if there are data wiaing
		if (buffer_incompleteData_len > 0)
		{
			// looks like there is unprocessed data in our buffer
			memcpy(buffer_incompleteData + buffer_incompleteData_len, buffer_withHeader, startOfCurrentFrame);
			buffer_incompleteData_len += *buffer_withHeader_len;
			// so our buffer_incomplete is now infact complete
			bbFrameID = (uint8_t)buffer_incompleteData[3];
			datafield_length = buffer_incompleteData[8];
			datafield_length = (datafield_length << 8) & 0xff00;
			datafield_length += ((uint16_t)buffer_incompleteData[9]) & 0x00ff;
			datafield_length = datafield_length / 8;

			// time to push it to the outgoing buffer
			memcpy(buffer_withoutHeader + *buffer_withoutHeader_len, buffer_incompleteData + 14, datafield_length);
			*buffer_withoutHeader_len = datafield_length;

			buffer_incompleteData_len = 0;
		}

		// since the marker now has been found, let's extract BB Header data
		datafield_length = buffer_withHeader[startOfCurrentFrame + 8];
		datafield_length = (datafield_length << 8) & 0xff00;
		datafield_length += ((uint16_t)buffer_withHeader[startOfCurrentFrame + 9]) & 0x00ff;
		datafield_length = datafield_length / 8;
		// todo SNR
		// todo MODCOD
		bbFrameID = (uint8_t)buffer_withHeader[startOfCurrentFrame + 3];
		// let's make sure that the bbFrame is complete
		startOfNextFrame = startOfCurrentFrame + datafield_length + 14;
		// we need to avoid "index out of range"
		if (startOfNextFrame < *buffer_withHeader_len)
		{
			if ((unsigned char)buffer_withHeader[startOfNextFrame] == 0xb8)
			{
				// so just copy to the outgoing buffer
				memcpy(buffer_withoutHeader + *buffer_withoutHeader_len, buffer_withHeader + startOfCurrentFrame + 14, datafield_length);
				*buffer_withoutHeader_len += datafield_length;
				// done working with the current frame
				// time to move to the nex one
				startOfCurrentFrame = startOfNextFrame;
				prev_bbFrameID = bbFrameID;
			}
			else
			{
				std::cout << "Anomaly detected: L3 Sync marker not at expected location. " << std::endl; // notify user. 
																										 // marker was supposed to be here. The only reason 
																										 // it is not expected at the present locaqtion is,
																										 // a. data loss occured. we will find the marker earlier.
																										 //    so we need to look for it.
																										 //    copy what ever data we can salvage out of it. maybe we'll get lucky and few complete TF. 
				bool haveWeExperienceDataLoss = false;
				for (int iteratorr = startOfNextFrame; iteratorr > startOfCurrentFrame + 14; iteratorr--)
				{
					if ((unsigned char)buffer_withHeader[iteratorr] == 0xb8)
					{
						// definitely we have experienced some data loss. 
						haveWeExperienceDataLoss = true;
						startOfNextFrame = iteratorr;
						break;
					}
				}
				if (!haveWeExperienceDataLoss)
				{
					// b. the marker moved forward. because, somehow garbahe got inside the stream. 
					//    which is very strange. need to alert the user. 
					//

					// this is quite rare. first the marker isn't where it was supposed to be. we have tons of data in the buffer...
					// also, strangely, there is no 0xB8 in the user data as well. strange world. 
					// since we can't find the next marker. we will just save the data and look for the next marker in the buffer.
					memcpy(buffer_withoutHeader + *buffer_withoutHeader_len, buffer_withHeader + startOfCurrentFrame + 14, datafield_length);
					*buffer_withoutHeader_len += datafield_length;
					// done working with the current frame
					// time to move to the nex one
					for (int iteratorr = startOfNextFrame; iteratorr < *buffer_withHeader_len; iteratorr++)
					{
						startOfNextFrame = iteratorr; // if the marker isn't present at all, this is supposed to cause the loop to break. the bigger one. todo test it with a file. 
						if ((unsigned char)buffer_withHeader[iteratorr] == 0xb8)
						{
							break;
						}
					}
				}
				else
				{
					// a.data loss occured.
					// we are going to grab whatever available. 
					memcpy(buffer_withoutHeader + *buffer_withoutHeader_len, buffer_withHeader + startOfCurrentFrame + 14, startOfNextFrame - startOfCurrentFrame - 14);
					*buffer_withoutHeader_len = *buffer_withoutHeader_len + startOfNextFrame - startOfCurrentFrame - 14;
				}
				// done processing the weird case where marker was expected but it was found either forward or backward
				startOfCurrentFrame = startOfNextFrame;
				prev_bbFrameID = bbFrameID;
			}
		}
		else
		{
			// incomplete
			// 2 scenarios
			// 1. We have the full datafield. but it is just perfect.
			// and the B8 marker is in the next batch
			// 2. We don't have the full data field. only partial.
			// in such case, we need to wait for the next batch.
			// find the marker and concatenate the data. 
			// case 1 >>
			if (startOfNextFrame == *buffer_withHeader_len)
			{
				// perfect buffer
				// so the data field is complete
				memcpy(buffer_withoutHeader + *buffer_withoutHeader_len, buffer_withHeader + startOfCurrentFrame + 14, datafield_length);
				*buffer_withoutHeader_len += datafield_length;
			}
			else // case 2 >>
			{
				// so definitely some of the user data we are
				// after has overflowed to the next buffer
				// so we copy what we have to our own buffer
				/*
				63 64
				b8ba...............................|..............b8ba
				start of the frame
				length of the icoming
				*/
				buffer_incompleteData_len = *buffer_withHeader_len - startOfCurrentFrame;
				memcpy(buffer_incompleteData, buffer_withHeader + startOfCurrentFrame, buffer_incompleteData_len);
			}
			// break out for the loop
			break;
		}
	}
	// loop end while (startOfCurrentFrame < *lengthOfIncoming)
}


void bbProcessor::stripBB(
	std::queue<s>& q_dataToProcess,
	std::mutex& mut_main2support,
	std::condition_variable& cv,
	std::atomic<bool>& finished)
{
#ifdef _DEBUG
	std::cout << "\t\t Starting Thread: BB Process. \n";
#endif
	//messageLogger bb_msgLog(filename_c, "_bb.log\0");
	//bb_msgLog.fileInit();
	//bb_msgLog.logMessage("test.");
	bool somethingWaitingToBeProcessed = false;
	char* buf_bbFrame = new char[c_BufSize];
	char* buf_extractedData = new char[c_BufSize * 2];
	int* buf_bbFrame_len = new int;
	int* buf_extractedData_len = new int;
	*buf_bbFrame_len = c_BufSize;
	*buf_extractedData_len = c_BufSize * 2;
	//FILE * outputFile_extractedData;
	//fopen_s(&outputFile_extractedData, "bbProcess.bin", "wb");

   // FILE * outputFile_debug;
	//fopen_s(&outputFile_debug, "bbProcess_dg.bin", "wb");

	// queue and other things for another thread
	std::queue<s> data2;
	std::condition_variable cv2;
	std::mutex mut2;
	std::atomic<bool> finished2 = false;

	//spin up the thread
	auto tfObj = tfProcessor();
	tfObj.filename_c = filename_c;
	std::thread worker2 = std::thread(
		&tfProcessor::sanitizeTf,
		&tfObj,
		std::ref(data2),
		std::ref(mut2),
		std::ref(cv2),
		std::ref(finished2)
	);

	while (true)
	{
		{
			std::unique_lock<std::mutex> lock{ mut_main2support };
			cv.wait(lock, [&] { return !q_dataToProcess.empty() || finished; });

			if (!q_dataToProcess.empty())
			{
				// void * memcpy ( void * destination, const void * source, size_t num )
				memcpy(buf_bbFrame, q_dataToProcess.front().pointerToByte, q_dataToProcess.front().numOfByte);
				*buf_bbFrame_len = q_dataToProcess.front().numOfByte;
				somethingWaitingToBeProcessed = true;
				// dbug file save
				// save directly from q. todo clean up
				//::fwrite(q_dataToProcess.front().pointerToByte, sizeof(char), q_dataToProcess.front().numOfByte, outputFile_debug);
				delete q_dataToProcess.front().pointerToByte;
				q_dataToProcess.pop();
			}// if !data.empty()
		}// mutex is out of scope
		 // time to process the data
		if (somethingWaitingToBeProcessed)
		{
			work(
				buf_bbFrame,
				buf_bbFrame_len,
				buf_extractedData,
				buf_extractedData_len);
			//	if (*buf_extractedData_len != 0)
				//{	
					//::fwrite(buf_extractedData, sizeof(char), *buf_extractedData_len, outputFile_extractedData);
				//}
			somethingWaitingToBeProcessed = false; // because at this point every thing is processed.
			// need to send this processed data to the other thread
			{
				// seperate scope for mutex
				std::lock_guard<std::mutex> lock{ mut2 };
				data2.emplace(new char[*buf_extractedData_len], *buf_extractedData_len);
				// memcopy
				memcpy(data2.back().pointerToByte, buf_extractedData, *buf_extractedData_len);
				cv2.notify_all();
			}
		}
		// break out from infinite loop
		if (finished)
		{
			break;
		}
	} // end of infinited loop
	 // before ending, one last check if queue is empty
	while (!q_dataToProcess.empty())
	{
		memcpy(buf_bbFrame, q_dataToProcess.front().pointerToByte, q_dataToProcess.front().numOfByte);
		*buf_bbFrame_len = q_dataToProcess.front().numOfByte;
		// dbug file save
		// save directly from q. todo clean up
		//::fwrite(q_dataToProcess.front().pointerToByte, sizeof(char), q_dataToProcess.front().numOfByte, outputFile_debug);
		q_dataToProcess.pop();
		// do the work
		work(
			buf_bbFrame,
			buf_bbFrame_len,
			buf_extractedData,
			buf_extractedData_len);
		//if (*buf_extractedData_len != 0)
		 //{
		//	 ::fwrite(buf_extractedData, sizeof(char), *buf_extractedData_len, outputFile_extractedData);
		 //}
		 // need to send this processed data to the other thread
		{
			// seperate scope for mutex
			std::lock_guard<std::mutex> lock{ mut2 };
			data2.emplace(new char[*buf_extractedData_len], *buf_extractedData_len);
			memcpy(data2.back().pointerToByte, buf_extractedData, *buf_extractedData_len);
			cv2.notify_all();
		}
	}
	// flush incomplete_data_buffer
	if (buffer_incompleteData_len != 0)
	{
		// ::fwrite(buffer_incompleteData, sizeof(char), buffer_incompleteData_len, outputFile_extractedData);

		std::lock_guard<std::mutex> lock{ mut2 };
		data2.emplace(new char[buffer_incompleteData_len], buffer_incompleteData_len);
		memcpy(data2.back().pointerToByte, buffer_incompleteData, buffer_incompleteData_len);
		cv2.notify_all();
	}

	finished2 = true;
	cv2.notify_all();
#ifdef _DEBUG
	std::cout << "\t\t Waiting for tf_process to finish. \n";
#endif
	worker2.join();
#ifdef _DEBUG
	std::cout << "\t\t End BB Process. \n";
#endif
	//fclose(outputFile_extractedData);
	//fclose(outputFile_debug);

	// deallocate heap memory
	delete[] buf_bbFrame;
	delete[] buf_extractedData;
	delete buf_bbFrame_len;
	delete buf_extractedData_len;
}
// todo too many branches. can we make it better???
