#include "tfProcess.h"


// dummy function used to setup unit test.
int tfProcessor::add(int a, int b) { return a + b; }

uint16_t tfProcessor::crc(char *buffer, int buffer_len)
{
	uint16_t crc = 0xffff;

	for (size_t i = 0; i < buffer_len; i++)
	{
		crc = crc ^ ((buffer[i] << 8)&0xff00);
		crc = crc & 0xFFFF;
		for (size_t j = 0; j < 8; j++)
		{
			if ((crc & 0x8000) == 0)
			{
				crc = ((crc << 1) & 0xfffe);
			}
			else
			{
				crc = ((crc << 1)&0xfffe) ^ 0x1021;
			}
			crc = crc & 0xFFFF;
		}
	}
	return crc;
}

void tfProcessor::tfWork(
	char *buffer_dirty,
	int *buffer_dirty_len,
	char *buffer_clean,
	int *buffer_clean_len)
{
	//messageLogger* tf_msgLog = new messageLogger(filename_c, "_tf.log\0");
	//tf_msgLog->fileInit();
	//tf_msgLog->logMessage("test from tf_work.");
	bool isAsmFound = false;
	bool isNextAsmFound = false;
	int currentAsmIndex;
	int nextAsmIndex;
	*buffer_clean_len = 0;
	uint16_t checksum_given;
	uint16_t checksum_calculated;
 
	for (int i = 0; i < ((*buffer_dirty_len) - 4); i++)
	{
		if ((unsigned char)buffer_dirty[i] == 0x1a)
		{
			if ((unsigned char)buffer_dirty[i + 1] == 0xcf)
			{
				if ((unsigned char)buffer_dirty[i + 2] == 0xfc)
				{
					if ((unsigned char)buffer_dirty[i + 3] == 0x1d)
					{
						// this is it. ASM found @i.
						isAsmFound = true;
						currentAsmIndex = i;
						break;
						// on a side note,
						// Of all the test I've done so far, this nested
						// if structure is the fastest.
					}
				}
			}
		}
	}

	if (isAsmFound)
	{
		//std::cout << "a";
		// check if data was waiting.
		if (*dataStorage_len > 0)
		{ // if so, append
			//std::cout << "b";
			memcpy(dataStorage + *dataStorage_len, buffer_dirty, currentAsmIndex);
			*dataStorage_len += currentAsmIndex;
			// do a sanity check
			if (*dataStorage_len >= 0x804)
			{
				//std::cout << "c";
				// we will only bother to check if the buffer has a full TF.
				checksum_calculated = crc(dataStorage + 4, 2046);
				checksum_given = ((dataStorage[0x802] << 8)&0xff00) + ((dataStorage[0x803])&0x00ff);
				if (checksum_calculated == checksum_given)
				{
					memcpy(buffer_clean, dataStorage, 0x804);
					*buffer_clean_len = 0x804;
				}
				else {
			//std::cout << "\t\t\t\t marker was found. and data was waiting. ";
#ifdef _DEBUG
					std::cout << "\t\t\t\t CRC mismatch. Given: ";
					std::cout << std::hex << checksum_given;
					std::cout << " Calculated: ";
					std::cout << std::hex << checksum_calculated;
					std::cout << std::endl;
#endif
				}
				// since the dataStorage was => 0x804
				// we will now clear the storage space. as data has been processed.
				*dataStorage_len = 0;
			}
			else
			{
				//std::cout << "d";
				*dataStorage_len = 0;
			}	
		} // end of if (*dataStorage_len > 0)
		while (currentAsmIndex < *buffer_dirty_len)
		{
			nextAsmIndex = currentAsmIndex + 0x804;
			// let's find out if this is a complet tf,
			if (nextAsmIndex == *buffer_dirty_len)
			{
				//std::cout << "e";
				// perfect buffer
				// do a crc check. save if complete
				checksum_calculated = crc(buffer_dirty + currentAsmIndex + 4, 2046);
				checksum_given = ((buffer_dirty[currentAsmIndex + 0x802] << 8)&0xff00) + ((buffer_dirty[currentAsmIndex + 0x803])&0x00ff);
				if (checksum_calculated == checksum_given)
				{
					memcpy(buffer_clean + *buffer_clean_len, buffer_dirty + currentAsmIndex, 0x804);
					*buffer_clean_len += 0x804;
				}
				else 
				{
#ifdef _DEBUG
					//std::cout << "\t\t\t\t check crc of a perfet buffer." << std::endl;
					std::cout << "\t\t\t\t CRC mismatch. Given: ";
					std::cout << std::hex << checksum_given;
					std::cout << " Calculated: ";
					std::cout << std::hex << checksum_calculated;
					std::cout << std::endl;
#endif
				}
				// break loop
				currentAsmIndex = *buffer_dirty_len + 10;
			} // end. case: perfect buffer.
			else
			{
				//std::cout << "f"; 
				if (nextAsmIndex < (*buffer_dirty_len) - 4)
				{
					//std::cout << "g";
					// plenty of data available, let's check
					// do a crc check. save if complete
					checksum_calculated = crc(buffer_dirty + currentAsmIndex + 4, 2046);
					checksum_given = (((buffer_dirty[currentAsmIndex + 0x802]) << 8) & 0xff00) + ((buffer_dirty[currentAsmIndex + 0x803]) & 0x00ff);
					//std::cout << "check ";
					//std::cout << std::hex << (int) buffer_dirty[currentAsmIndex + 0x802] << " \n";
					if (checksum_calculated == checksum_given)
					{
						memcpy(buffer_clean + *buffer_clean_len, buffer_dirty + currentAsmIndex, 0x804);
						*buffer_clean_len += 0x804;
					}
					else 
					{
#ifdef _DEBUG
						//std::cout << "\t\t\t\t check crc of comfort buffer";
						std::cout << "\t\t\t\t CRC mismatch. Given: ";
						std::cout << std::hex << checksum_given;
						std::cout << " Calculated: ";
						std::cout << std::hex << checksum_calculated;
						std::cout << std::endl; // << I must have drank too many coffee that day!
#endif
					}
					isNextAsmFound = false;
					if ((unsigned char)buffer_dirty[nextAsmIndex] == 0x1a)
					{
						if ((unsigned char)buffer_dirty[nextAsmIndex + 1] == 0xcf)
						{
							if ((unsigned char)buffer_dirty[nextAsmIndex + 2] == 0xfc)
							{
								if ((unsigned char)buffer_dirty[nextAsmIndex + 3] == 0x1d)
								{
									// Yes. indeed the TF is complete.
									isNextAsmFound = true;

									// prepare for the next loop
									currentAsmIndex = nextAsmIndex;
								}
							}
						}
					}
					if (!isNextAsmFound)
					{
						for (int i = currentAsmIndex + 4; i + 3 < *buffer_dirty_len; i++)
						{
							if ((unsigned char)buffer_dirty[i] == 0x1a)
							{
								if ((unsigned char)buffer_dirty[i + 1] == 0xcf)
								{
									if ((unsigned char)buffer_dirty[i + 2] == 0xfc)
									{
										if ((unsigned char)buffer_dirty[i + 3] == 0x1d)
										{
											currentAsmIndex = i;
											isNextAsmFound = true;
											break;
										}
									}
								}
							}
						}
					}
					if (!isNextAsmFound)
					{
						// this means there are no more asm in the buffer
						// break the loop
						currentAsmIndex = *buffer_dirty_len + 10;
					}
				} // end. case: buffer was comfortably inside.
				else
				{
					//std::cout << "h";
					// next buffer is coming next time.
					// we need to save this in another temporary buffer. and wait for the next batch of data.
					memcpy(dataStorage, buffer_dirty+currentAsmIndex, *buffer_dirty_len - currentAsmIndex);
					*dataStorage_len = *buffer_dirty_len - currentAsmIndex;
					currentAsmIndex = *buffer_dirty_len + 20; // break loop



					// on rare occasion, if there is 4 bytes of ASM at the end of buffer,
					// the data is mistakenly stored in buffer.
					// and one of the TF eventually gets lost in processing. so....
					// todo eita ki chilo? mone nai. 
					if (*dataStorage_len >= 0x804)
					{
						//std::cout << "i";
						checksum_calculated = crc(dataStorage + 4, 2046);
						checksum_given = ((dataStorage[0x802] << 8)&0xff00) + ((dataStorage[0x803])&0x00ff);
						if (checksum_calculated == checksum_given)
						{
							// send the TF to output
							memcpy(buffer_clean + *buffer_clean_len, dataStorage, 0x804);
							*buffer_clean_len += 0x804;
						}
						else 
						{
#ifdef _DEBUG
							//std::cout << "\t\t\t\t i forgot what section does." << std::endl;
							std::cout << "\t\t\t\t CRC mismatch. Given: ";
							std::cout << std::hex << checksum_given;
							std::cout << " Calculated: ";
							std::cout << std::hex << checksum_calculated;
							std::cout << std::endl;
#endif
						}
						// move thos 4/3/2/1 bytes of ASM to the beginning of the data storage buffer
						memcpy(dataStorage, dataStorage + 0x804, *dataStorage_len - 0x804);
						*dataStorage_len -= 0x804;
					}
				}
			}
		}
	} // if marker was found
	else
	{
		//std::cout << "i";
		// asm marker not found
		// perhaps we were expecting data
		if (*dataStorage_len > 0)
		{
			//std::cout << "j";
			// correct. we have been waiting for more data.
			memcpy(dataStorage + *dataStorage_len, buffer_dirty, *buffer_dirty_len);
			*dataStorage_len += *buffer_dirty_len;
			if (*dataStorage_len >= 0x804)
			{
				//std::cout << "k";
				// we will only bother to check if the buffer has a full TF.

				checksum_calculated = crc(dataStorage + 4, 2046);
				checksum_given = ((dataStorage[0x802] << 8)&0xff00) + ((dataStorage[0x803])&0x00ff);
				if (checksum_calculated == checksum_given)
				{					 
					memcpy(buffer_clean, dataStorage, 0x804);
					*buffer_clean_len = 0x804;
				}
#ifdef _DEBUG
				else 
				{
					//std::cout << "\t\t\t\t Marker was not found. but data was waiting.\n";
					std::cout << "\t\t\t\t CRC mismatch. Given: ";
					std::cout << std::hex << checksum_given;
					std::cout << " Calculated: ";
					std::cout << std::hex << checksum_calculated;
					std::cout << std::endl;
				}
#endif
				// since the dataStorage was => 0x804
				// we will now clear the storage space. as data has been processed.
				*dataStorage_len = 0;
			}
			// else{
			// the buffer isn't even a full TF.
			// don't bother to process it now. wait for more data.}
		} // if (*dataStorage_len > 0)
	}	  // end of "the marker was not found in this buffer"
	//delete tf_msgLog;
} // end of tfWork

void tfProcessor::sanitizeTf(
	std::queue<s> &q_tf,
	std::mutex &mut_tf,
	std::condition_variable &cv_tf,
	std::atomic<bool> &ab_tf)
{
	bool somethingWaitingToBeProcessed = false;
	char *buf_tf = new char[c_BufSize * 2];
	char *buf_sanitized = new char[c_BufSize * 2];
	int *buf_tf_len = new int;
	int *buf_sanitized_len = new int;

	//char* temp = new char[64];
	//filenameAppend(filename_c, temp);
#ifdef _DEBUG
	std::cout << "tf::filename_c: " << filename_c << std::endl;
#endif
	FILE *outputFile_tf;
	auto fileopen_error_code = fopen_s(&outputFile_tf, filename_c, "wb");
	if (fileopen_error_code != 0)
	{
		// todo improve
		std::cout << "output file open error. \n";
	}

	// file for debugging
	//FILE *outputFile_tf_db;
	//fopen_s(&outputFile_tf_db, "tf_db.bin", "wb");

	//messageLogger* tf_msgLog = new messageLogger(filename_c, "_tf.log\0");
	//tf_msgLog->fileInit();
	//tf_msgLog->logMessage("test from tf.");
	//delete tf_msgLog;

#ifdef _DEBUG
	std::cout << "\t\t\t\t Starting thread: TF Process. ";
#endif
	while (true)
	{
		{
			// dedicated scope for the mutex
			std::unique_lock<std::mutex> lock{mut_tf};
			cv_tf.wait(lock, [&]
					   { return !q_tf.empty() || ab_tf; });

			if (!q_tf.empty())
			{
				// copy to local buffer
				// void * memcpy ( void * destination, const void * source, size_t num )
				memcpy(buf_tf, q_tf.front().pointerToByte, q_tf.front().numOfByte);
				*buf_tf_len = q_tf.front().numOfByte;
				somethingWaitingToBeProcessed = true;
				// dbug file save
				// save directly from q. todo clean up
				//::fwrite(q_tf.front().pointerToByte, sizeof(char), q_tf.front().numOfByte, outputFile_tf_db);
				delete q_tf.front().pointerToByte; // deallocate the heap memory. 
				q_tf.pop();
			} // if !data.empty()
		}	  // mutex is out of scope
		if (somethingWaitingToBeProcessed)
		{
			// process the data here
			tfWork(buf_tf, buf_tf_len, buf_sanitized, buf_sanitized_len);
			//if (*buf_tf_len != 0)
			//{
			//	::fwrite(buf_tf, sizeof(char), *buf_tf_len, outputFile_tf_db);
			//}
			if (*buf_sanitized_len != 0)
			{
				::fwrite(buf_sanitized, sizeof(char), *buf_sanitized_len, outputFile_tf);
			}
			somethingWaitingToBeProcessed = false;
		}
		if (ab_tf)
		{
			break;
		}
	} // end of infinite loop

	while (!q_tf.empty())
	{
		// copy to local buffer
		// void * memcpy ( void * destination, const void * source, size_t num )
		memcpy(buf_tf, q_tf.front().pointerToByte, q_tf.front().numOfByte);
		*buf_tf_len = q_tf.front().numOfByte;
		// dbug file save
		// save directly from q. todo clean up
		//::fwrite(q_tf.front().pointerToByte, sizeof(char), q_tf.front().numOfByte, outputFile_tf_db);
		delete q_tf.front().pointerToByte; // deallocate the heap memory.
		q_tf.pop();
		tfWork(buf_tf, buf_tf_len, buf_sanitized, buf_sanitized_len);
		/*if (*buf_tf_len != 0)
		{
			::fwrite(buf_tf, sizeof(char), *buf_tf_len, outputFile_tf_db);
		}*/
		if (*buf_sanitized_len != 0 && fileopen_error_code == 0)
		{
			::fwrite(buf_sanitized, sizeof(char), *buf_sanitized_len, outputFile_tf);
		}
	}
	if (outputFile_tf)
	{
		fclose(outputFile_tf);
	}
	//fclose(outputFile_tf_db);
	delete[] buf_tf ;
	delete[] buf_sanitized ;
	delete buf_tf_len;
	delete buf_sanitized_len;
#ifdef _DEBUG
	std::cout << "\t\t\t\t End TF Process.\n";
#endif
} // end of sanitize_tf