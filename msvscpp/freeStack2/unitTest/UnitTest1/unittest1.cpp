#include "CppUnitTest.h"
#include "tfProcess.h"
#include "bbProcess.h"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
	TEST_CLASS(UnitTest1)
	{
	public:

		// unit test for unit test. Inception ;)
		TEST_METHOD(TestMethod1)
		{
			auto myObj = tfProcessor();
			int calculated = myObj.add(3, 2);
			int expected = 5;
			Assert::AreEqual(calculated, expected);
		}

		TEST_METHOD(TestMethod2)
		{
			auto myObj = bbProcessor();
			int calculated = myObj.substract(3, 2);
			int expected = 1;
			Assert::AreEqual(calculated, expected);
		}

		// check for CRC
		TEST_METHOD(crc1)
		{
			auto myobj = tfProcessor();
			char buffer[] = { 0, 1, 2, 3, 4, 5 };
			int calculated = (int)myobj.crc(buffer, 6);
			int expected = 35864;
			Assert::AreEqual(calculated, expected);
		}

		// check for CRC
		TEST_METHOD(crc2)
		{
			char* buffer = new char[0x802];
			buffer[0] = 0x1a;
			buffer[1] = 0xfc;
			buffer[2] = 0xfc;
			buffer[3] = 0x1d;
			buffer[4] = 0x40;
			buffer[5] = 0x3f;
			buffer[6] = 0x01;
			buffer[7] = 0xe3;
			buffer[8] = 0x54;
			buffer[9] = 0x00;
			buffer[10] = 0x07;
			buffer[11] = 0xfe;
			buffer[12] = 0x0b;
			buffer[13] = 0x7a;
			buffer[14] = 0x00;
			buffer[15] = 0x23;
			buffer[16] = 0x00;
			buffer[17] = 0x0d;
			buffer[18] = 0x04;
			buffer[19] = 0xa1;
			buffer[20] = 0x00;
			buffer[21] = 0x15;
			buffer[22] = 0x0c;
			buffer[23] = 0xbd;
			buffer[24] = 0x07;
			buffer[25] = 0xfb;
			buffer[26] = 0x07;
			buffer[27] = 0xde;
			buffer[28] = 0x00;
			buffer[29] = 0x00;
			buffer[30] = 0x00;
			buffer[31] = 0x00;
			buffer[32] = 0x24;
			buffer[33] = 0xff;
			buffer[34] = 0x01;
			buffer[35] = 0x01;
			buffer[36] = 0x00;
			buffer[37] = 0x37;
			buffer[38] = 0x10;
			buffer[39] = 0x32;
			buffer[40] = 0x17;
			buffer[41] = 0x00;
			buffer[42] = 0x00;
			buffer[43] = 0x00;
			buffer[44] = 0xe1;
			buffer[45] = 0x21;
			buffer[46] = 0x12;
			buffer[47] = 0x06;
			for (int i = 48; i < 0x802; i++)
			{
				buffer[i] = 0x00;
			}
			auto myobj = tfProcessor();
			int calculated = (int)myobj.crc(buffer + 4, 0x802 - 4);
			int expected = 0x7d25;
			Assert::AreEqual(calculated, expected);
		}

		// function to write ASM in a buffer.
		void loadAsm(char* buffer)
		{
			buffer[0] = (uint8_t)0x1a;
			buffer[1] = (uint8_t)0xcf;
			buffer[2] = (uint8_t)0xfc;
			buffer[3] = (uint8_t)0x1d;
		}

		void loadCrc(char* buffer)
		{
			auto myobj = tfProcessor();
			int calculated = (int)myobj.crc(buffer + 4, 0x804 - 4 - 2);
			buffer[0x803] = calculated & 0x00FF;
			buffer[0x802] = (calculated >> 8) & 0x00FF;
		}

		// what happens when the buffer has one TF. Perfect in every way
		TEST_METHOD(perfect1)
		{
			char* original = new char[0x804];
			char* processed = new char[0x804];
			int* original_len = new int;
			int* processed_len = new int;
			*original_len = 0x804;
			loadAsm(original);
			loadCrc(original);
			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			// convert the buffers to vectors
			std::vector<char> vec_original, vec_processed;
			vec_original.assign(original, original + *original_len);
			vec_processed.assign(processed, processed + *processed_len);

			Assert::IsTrue(vec_original == vec_processed);
		}

		// what happens when the buffer has 3 TF. Perfect in every way
		TEST_METHOD(perfect3)
		{
			char* original = new char[0x804 * 3];
			char* processed = new char[0x804 * 3];
			int* original_len = new int;
			int* processed_len = new int;
			*original_len = 0x804 * 3;
			loadAsm(original);
			loadAsm(original + 0x804);
			loadAsm(original + 0x804 * 2);
			loadCrc(original);
			loadCrc(original + 0x804);
			loadCrc(original + 0x804 * 2);
			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			// convert the buffers to vectors
			std::vector<char> vec_original, vec_processed;
			vec_original.assign(original, original + *original_len);
			vec_processed.assign(processed, processed + *processed_len);

			Assert::IsTrue(vec_original == vec_processed);
		}

		// what happens when the buffer has 3 TF. but some random bytes in front
		TEST_METHOD(paddingInTheFront)
		{
			int* processed_len = new int;
			int* original_len = new int;
			*original_len = 0x804 * 4;
			char* original = new char[*original_len];
			char* processed = new char[*original_len];
			char* expected = new char[*original_len - 0x804];
			int* expected_len = new int;
			*expected_len = *original_len - 0x804;
			int const offset = 50;
			// construct the orignal
			loadAsm(original + offset);
			loadAsm(original + offset + 0x804);
			loadAsm(original + offset + 0x804 * 2);
			loadCrc(original + offset);
			loadCrc(original + offset + 0x804);
			loadCrc(original + offset + 0x804 * 2);
			Logger::WriteMessage("construction finished");
			// call the funtion
			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			Logger::WriteMessage("work finished");
			// construct the expected
			loadAsm(expected);
			loadAsm(expected + 0x804);
			loadAsm(expected + 0x804 * 2);
			loadCrc(expected);
			loadCrc(expected + 0x804);
			loadCrc(expected + 0x804 * 2);
			// convert the buffers to vectors
			std::vector<char> vec_processed, vec_expected;
			vec_processed.assign(processed, processed + *processed_len);
			vec_expected.assign(expected, expected + *expected_len);
			Logger::WriteMessage("checking");
			Assert::IsTrue(vec_expected == vec_processed);
		}

		// what happens when the buffer has 3 TF. but some random bytes in Back
		TEST_METHOD(paddingInTheBack)
		{
			int* processed_len = new int;
			int* original_len = new int;
			*original_len = 0x804 * 4;
			char* original = new char[*original_len];
			char* processed = new char[*original_len];
			char* expected = new char[*original_len - 0x804];
			int* expected_len = new int;
			*expected_len = *original_len - 0x804;
			int const offset = 0;
			// construct the orignal
			loadAsm(original + offset);
			loadAsm(original + offset + 0x804);
			loadAsm(original + offset + 0x804 * 2);
			loadCrc(original + offset);
			loadCrc(original + offset + 0x804);
			loadCrc(original + offset + 0x804 * 2);
			// call the funtion
			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			// construct the expected
			loadAsm(expected);
			loadAsm(expected + 0x804);
			loadAsm(expected + 0x804 * 2);
			loadCrc(expected);
			loadCrc(expected + 0x804);
			loadCrc(expected + 0x804 * 2);
			// convert the buffers to vectors
			std::vector<char> vec_original, vec_processed, vec_expected;
			vec_original.assign(original, original + *original_len);
			vec_processed.assign(processed, processed + *processed_len);
			vec_expected.assign(expected, expected + *expected_len);

			Assert::IsTrue(vec_expected == vec_processed);
		}

		// what happens when the buffer has 3 TF. but some bytes in the second TF is missing.
		TEST_METHOD(dropInTheMiddle)
		{
			int* processed_len = new int;
			int* original_len = new int;
			*original_len = 0x804 * 4;
			char* original = new char[*original_len];
			char* processed = new char[*original_len];
			char* expected = new char[*original_len - 0x804];
			int* expected_len = new int;
			*expected_len = 0x804 * 2;
			int const offset = 50;
			// construct the orignal
			loadAsm(original + offset);
			loadCrc(original + offset);

			loadAsm(original + offset + 0x804);

			loadAsm(original + offset + 0x804 * 2 - 50);
			loadCrc(original + offset + 0x804 * 2 - 50);
			// call the funtion
			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			// construct the expected
			loadAsm(expected);
			loadCrc(expected);
			loadAsm(expected + 0x804);
			loadCrc(expected + 0x804);
			// convert the buffers to vectors
			std::vector<char> vec_processed, vec_expected;
			vec_processed.assign(processed, processed + *processed_len);
			vec_expected.assign(expected, expected + *expected_len);

			Assert::IsTrue(vec_expected == vec_processed);
		}

		TEST_METHOD(invalid_input)
		{
			char* original = new char[0x804];
			char* processed = new char[0x804];
			int* original_len = new int;
			int* processed_len = new int;
			*original_len = 0x804;
			loadAsm(original);
			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			Assert::IsTrue(*processed_len == 0);
		}

		// case: input buffer is shorter that 0x804
		TEST_METHOD(short_buffer)
		{
			char* original = new char[0x402];
			char* processed = new char[0x402];
			int* original_len = new int;
			int* processed_len = new int;
			*original_len = 0x402;
			loadAsm(original);
			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			Assert::IsTrue(0 == *processed_len);
		}

		// case: padding in the middle
		TEST_METHOD(gapInTheMiddle)
		{
			int* processed_len = new int;
			int* original_len = new int;
			*original_len = 0x804 * 4;
			char* original = new char[*original_len];
			char* processed = new char[*original_len];
			char* expected = new char[*original_len - 0x804];
			int* expected_len = new int;
			*expected_len = 0x804 * 3;
			int const offset = 0x804;
			// construct the orignal
			loadAsm(original);
			loadCrc(original);

			loadAsm(original + 0x804);
			loadCrc(original + 0x804);

			loadAsm(original + offset + 0x804 * 2);
			loadCrc(original + offset + 0x804 * 2);
			// call the funtion
			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			// construct the expected
			loadAsm(expected);
			loadCrc(expected);
			loadAsm(expected + 0x804);
			loadCrc(expected + 0x804);
			loadAsm(expected + 0x804 * 2);
			loadCrc(expected + 0x804 * 2);
			// convert the buffers to vectors
			std::vector<char> vec_processed, vec_expected;
			vec_processed.assign(processed, processed + *processed_len);
			vec_expected.assign(expected, expected + *expected_len);

			Assert::IsTrue(vec_expected == vec_processed);
		}

		// case: padding in the front, middle and back.
		TEST_METHOD(gapInBetweenTF)
		{
			int* processed_len = new int;
			int* original_len = new int;
			*original_len = 0x804 * 4;
			char* original = new char[*original_len];
			char* processed = new char[*original_len];
			char* expected = new char[*original_len - 0x804];
			int* expected_len = new int;
			*expected_len = 0x804 * 3;
			int const offset = 50;
			// construct the orignal
			loadAsm(original + offset);
			loadCrc(original + offset);

			loadAsm(original + offset + 0x804);
			loadCrc(original + offset + 0x804);

			loadAsm(original + offset * 2 + 0x804 * 2);
			loadCrc(original + offset * 2 + 0x804 * 2);
			// call the funtion
			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			// construct the expected
			loadAsm(expected);
			loadCrc(expected);
			loadAsm(expected + 0x804);
			loadCrc(expected + 0x804);
			loadAsm(expected + 0x804 * 2);
			loadCrc(expected + 0x804 * 2);
			// convert the buffers to vectors
			std::vector<char> vec_processed, vec_expected;
			vec_processed.assign(processed, processed + *processed_len);
			vec_expected.assign(expected, expected + *expected_len);

			Assert::IsTrue(vec_expected == vec_processed);
		}
		// case: wrong crc.
		TEST_METHOD(wrong_crc)
		{
			char* original = new char[0x804];
			char* processed = new char[0x804];
			int* original_len = new int;
			int* processed_len = new int;
			*original_len = 0x804;
			loadAsm(original);
			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			Assert::IsTrue(*processed_len == 0);
		}

		// case: 5 buffer. 5 TF each.
		TEST_METHOD(fiveByFive)
		{
			char* original = new char[0x804 * 5];
			char* processed = new char[0x804 * 5];
			int* original_len = new int;
			int* processed_len = new int;
			*original_len = 0x804 * 5;
			bool didItMatch = true;
			for (int i = 0; i < 5; i++)
			{
				loadAsm(original + 0x804 * i);
				loadCrc(original + 0x804 * i);
			}
			std::vector<char> vec_original, vec_processed;
			vec_original.assign(original, original + *original_len);
			for (int i = 0; i < 5; i++)
			{
				auto myobj = tfProcessor();
				myobj.tfWork(original, original_len, processed, processed_len);
				vec_processed.assign(processed, processed + *processed_len);
				didItMatch = didItMatch & (vec_original == vec_processed);
			}

			Assert::IsTrue(didItMatch);
		}

		// case: TF as cut in the middle.
		TEST_METHOD(singleSplit)
		{
			char* original = new char[0x804];
			char* processed = new char[0x804];
			int* original_len = new int;
			int* processed_len = new int;
			*original_len = 0x804 / 2;
			loadAsm(original);
			loadCrc(original);
			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			myobj.tfWork(original + 0x804 / 2, original_len, processed, processed_len);
			// convert the buffers to vectors
			std::vector<char> vec_original, vec_processed;
			vec_original.assign(original, original + (*original_len * 2));
			vec_processed.assign(processed, processed + *processed_len);

			Assert::IsTrue(vec_original == vec_processed);
		}

		// case: it took 3 buffers for one TF.
		TEST_METHOD(doubleSplit)
		{
			char* original = new char[0x804];
			char* processed = new char[0x804];
			int* original_len = new int;
			int* processed_len = new int;
			*original_len = 0x804 / 3; // 2052 is divisible by 3.
			loadAsm(original);
			loadCrc(original);
			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			myobj.tfWork(original + 0x804 / 3, original_len, processed, processed_len);
			myobj.tfWork(original + ((0x804 / 3) * 2), original_len, processed, processed_len);
			// convert the buffers to vectors
			std::vector<char> vec_original, vec_processed;
			vec_original.assign(original, original + (*original_len * 3));
			vec_processed.assign(processed, processed + *processed_len);

			Assert::IsTrue(vec_original == vec_processed);
		}

		// case: the last value in buffer is the ASM
		TEST_METHOD(lastAsm4)
		{
			char* original = new char[0x808];
			char* processed = new char[0x808];
			int* original_len = new int;
			int* processed_len = new int;
			*original_len = 0x808;
			loadAsm(original);
			loadCrc(original);
			loadAsm(original + 0x804);
			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			// convert the buffers to vectors
			std::vector<char> vec_expected, vec_processed;
			vec_expected.assign(original, original + 0x804);
			vec_processed.assign(processed, processed + *processed_len);
			Assert::IsTrue(vec_expected == vec_processed);
		}

		// case: the last value in the buffer is the first three bytes of ASM
		TEST_METHOD(lastAsm3)
		{
			char* original = new char[0x807];
			char* processed = new char[0x807];
			int* original_len = new int;
			int* processed_len = new int;
			*original_len = 0x807;
			loadAsm(original);
			loadCrc(original);

			original[0x804] = (uint8_t)0x1a;
			original[0x805] = (uint8_t)0xcf;
			original[0x806] = (uint8_t)0xfc;

			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			// convert the buffers to vectors
			std::vector<char> vec_expected, vec_processed;
			vec_expected.assign(original, original + 0x804);
			vec_processed.assign(processed, processed + *processed_len);
			Assert::IsTrue(vec_expected == vec_processed);
		}

		// case: the last value in the buffer is the first two bytes of ASM
		TEST_METHOD(lastAsm2)
		{
			char* original = new char[0x806];
			char* processed = new char[0x806];
			int* original_len = new int;
			int* processed_len = new int;
			*original_len = 0x806;
			loadAsm(original);
			loadCrc(original);

			original[0x804] = (uint8_t)0x1a;
			original[0x805] = (uint8_t)0xcf;

			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			// convert the buffers to vectors
			std::vector<char> vec_expected, vec_processed;
			vec_expected.assign(original, original + 0x804);
			vec_processed.assign(processed, processed + *processed_len);
			Assert::IsTrue(vec_expected == vec_processed);
		}

		// case: the last value in the buffer is the first one bytes of ASM
		TEST_METHOD(lastAsm1)
		{
			char* original = new char[0x805];
			char* processed = new char[0x805];
			int* original_len = new int;
			int* processed_len = new int;
			*original_len = 0x805;
			loadAsm(original);
			loadCrc(original);

			original[0x804] = (uint8_t)0x1a;

			auto myobj = tfProcessor();
			myobj.tfWork(original, original_len, processed, processed_len);
			// convert the buffers to vectors
			std::vector<char> vec_expected, vec_processed;
			vec_expected.assign(original, original + 0x804);
			vec_processed.assign(processed, processed + *processed_len);
			Assert::IsTrue(vec_expected == vec_processed);
		}

		void bbFrameInit(char* buffer, int* expLen, uint8_t* id)
		{
			buffer[0] = (unsigned char)0xb8;
			buffer[3] = *id;
			uint16_t datafield_len_bits = (*expLen) * 8;
			buffer[9] = (uint8_t)datafield_len_bits & 0x00ff;
			buffer[8] = (uint8_t)((datafield_len_bits >> 8) & 0x00ff);

			for (int i = 14; i < *expLen + 14; i++)
			{
				buffer[i] = i;
			}
			buffer[*expLen - 1 + 14] = (unsigned char)0xb9; // just to check the ending. 
		}

		void expectedBufInit(char* buffer, int* dataLen)
		{
			for (int i = 14; i < *dataLen + 14; i++)
			{
				buffer[i - 14] = i;
			}
			buffer[*dataLen - 1] = (unsigned char)0xb9; // just to check the ending.
		}

		TEST_METHOD(bb_perfect1)
		{
			// varialble declarations
			char* buf_original;
			int* buf_original_len;
			char* buf_processed;
			int* buf_processed_len;
			int* datafield_len;
			uint8_t* bbFrameID;

			// dersired state
			bbFrameID = new uint8_t;
			*bbFrameID = 2;
			datafield_len = new int;
			*datafield_len = 8;

			buf_original_len = new int;
			*buf_original_len = *datafield_len + 14;

			// malloc
			buf_original = new char[*buf_original_len];
			buf_processed = new char[*buf_original_len];
			buf_processed_len = new int;

			// create the input
			bbFrameInit(buf_original, datafield_len, bbFrameID);

			// DUT
			auto myobj = bbProcessor();
			myobj.work(buf_original, buf_original_len, buf_processed, buf_processed_len, false);

			// turn them into vector
			std::vector<char> vec_original, vec_processed, vec_expected;
			vec_expected.assign(buf_original + 14, buf_original + 14 + *datafield_len);
			vec_processed.assign(buf_processed, buf_processed + *buf_processed_len);
			Assert::IsTrue(vec_expected == vec_processed);
		}

		TEST_METHOD(bb_perfect3)
		{
			// varialble declarations
			char* buf_original;
			int* buf_original_len;
			char* buf_processed;
			int* buf_processed_len;
			char* buf_expected;
			int* buf_expected_len;
			int* datafield_len;
			uint8_t* bbFrameID;

			// dersired state
			bbFrameID = new uint8_t;
			*bbFrameID = 2;
			datafield_len = new int;
			*datafield_len = 8;

			buf_original_len = new int;
			*buf_original_len = (*datafield_len + 14) * 3;

			// malloc
			buf_original = new char[*buf_original_len];
			buf_processed = new char[*buf_original_len];
			buf_processed_len = new int;
			buf_expected = new char[*buf_original_len];
			buf_expected_len = new int;

			// create the input
			bbFrameInit(buf_original, datafield_len, bbFrameID);
			bbFrameInit(buf_original + *datafield_len + 14, datafield_len, bbFrameID);
			bbFrameInit(buf_original + (*datafield_len + 14) * 2, datafield_len, bbFrameID);

			// create expected buffer
			expectedBufInit(buf_expected, datafield_len);
			expectedBufInit(buf_expected + *datafield_len, datafield_len);
			expectedBufInit(buf_expected + (*datafield_len) * 2, datafield_len);
			*buf_expected_len = *datafield_len * 3;

			// DUT
			auto myobj = bbProcessor();
			myobj.work(buf_original, buf_original_len, buf_processed, buf_processed_len, false);

			// turn them into vector
			std::vector<char> vec_original, vec_processed, vec_expected;
			vec_expected.assign(buf_expected, buf_expected + *buf_expected_len);
			vec_processed.assign(buf_processed, buf_processed + *buf_processed_len);
			Assert::IsTrue(vec_expected == vec_processed);
		}

		TEST_METHOD(bb_perfect1_split)
		{
			// what happens when there a single bb frame. but delivered in 2 buffers. 
			// varialble declarations
			char* buf_original;
			int* buf_original_len;
			char* buf_processed;
			int* buf_processed_len;
			char* buf_expected;
			int* buf_expected_len;
			int* datafield_len;
			uint8_t* bbFrameID;

			// dersired state
			bbFrameID = new uint8_t;
			*bbFrameID = 2;
			datafield_len = new int;
			*datafield_len = 18;

			buf_original_len = new int;
			*buf_original_len = (*datafield_len + 14) * 1;

			// malloc
			buf_original = new char[*buf_original_len];
			buf_processed = new char[*buf_original_len];
			buf_processed_len = new int;
			buf_expected = new char[*buf_original_len];
			buf_expected_len = new int;

			// create the input
			bbFrameInit(buf_original, datafield_len, bbFrameID);

			// create expected buffer
			expectedBufInit(buf_expected, datafield_len);
			*buf_expected_len = *datafield_len * 1;

			// DUT
			int* input_len;
			input_len = new int(16);
			auto myobj = bbProcessor();
			myobj.work(buf_original, input_len, buf_processed, buf_processed_len, false);
			*input_len = *buf_original_len - *input_len;
			myobj.work(buf_original + *input_len, input_len, buf_processed, buf_processed_len, false);

			// turn them into vector
			std::vector<char> vec_original, vec_processed, vec_expected;
			vec_expected.assign(buf_expected, buf_expected + *buf_expected_len);
			vec_processed.assign(buf_processed, buf_processed + *buf_processed_len);
			Assert::IsTrue(vec_expected == vec_processed);
		}

		TEST_METHOD(bb_perfect3_split)
		{
			// what happens when there are 3 perfect bb frame. 
			// but it got delivered by 2 buffer instead of one. 
			// varialble declarations
			char* buf_original;
			int* buf_original_len;
			char* buf_processed;
			int* buf_processed_len;
			char* buf_expected;
			int* buf_expected_len;
			int* datafield_len;
			int* input_len;
			uint8_t* bbFrameID;

			// dersired state
			bbFrameID = new uint8_t;
			*bbFrameID = 2;
			datafield_len = new int;
			*datafield_len = 8;

			buf_original_len = new int;
			*buf_original_len = (*datafield_len + 14) * 3;

			// malloc
			buf_original = new char[*buf_original_len];
			buf_processed = new char[*buf_original_len];
			buf_processed_len = new int;
			buf_expected = new char[*buf_original_len];
			buf_expected_len = new int;
			input_len = new int(0);

			// create the input
			bbFrameInit(buf_original, datafield_len, bbFrameID);
			bbFrameInit(buf_original + *datafield_len + 14, datafield_len, bbFrameID);
			bbFrameInit(buf_original + (*datafield_len + 14) * 2, datafield_len, bbFrameID);

			// create expected buffer
			expectedBufInit(buf_expected, datafield_len);
			expectedBufInit(buf_expected + *datafield_len, datafield_len);
			expectedBufInit(buf_expected + (*datafield_len) * 2, datafield_len);
			*buf_expected_len = *datafield_len * 3;

			// DUT
			*input_len = 14 + *datafield_len + 14 + 2;
			auto myobj = bbProcessor();
			myobj.work(buf_original, input_len, buf_processed, buf_processed_len, false);
			*input_len = *buf_original_len - *input_len;
			int temp = *buf_processed_len;
			myobj.work(buf_original + 14 + *datafield_len + 14 + 2, input_len, buf_processed + *datafield_len, buf_processed_len, false);
			//   -------------------------------------------             -------------------------------
			//   until this all sent                                     because it is expected the buf_processes already contains 1 complete datafield_len

			*buf_processed_len = *buf_processed_len + temp;

			// turn them into vector
			std::vector<char> vec_original, vec_processed, vec_expected;
			vec_expected.assign(buf_expected, buf_expected + *buf_expected_len);
			vec_processed.assign(buf_processed, buf_processed + *buf_processed_len);
			Assert::IsTrue(vec_expected == vec_processed);
		}

		// what happens when BB Frame has some garbage data infront?
		TEST_METHOD(bb_padding_front)
		{
			// varialble declarations
			char* buf_original;
			int* buf_original_len;
			char* buf_processed;
			int* buf_processed_len;
			char* buf_expected;
			int* buf_expected_len;
			int* datafield_len;
			uint8_t* bbFrameID;

			// dersired state
			bbFrameID = new uint8_t;
			*bbFrameID = 2;
			datafield_len = new int;
			*datafield_len = 8;

			buf_original_len = new int;
			*buf_original_len = (*datafield_len + 14) * 3;

			// malloc
			buf_original = new char[*buf_original_len];
			buf_processed = new char[*buf_original_len];
			buf_processed_len = new int;
			buf_expected = new char[*buf_original_len];
			buf_expected_len = new int;

			// create the input
			//bbFrameInit(buf_original, datafield_len, bbFrameID);
			bbFrameInit(buf_original + *datafield_len + 14, datafield_len, bbFrameID);
			bbFrameInit(buf_original + (*datafield_len + 14) * 2, datafield_len, bbFrameID);

			// create expected buffer
			expectedBufInit(buf_expected, datafield_len);
			expectedBufInit(buf_expected + *datafield_len, datafield_len);
			//expectedBufInit(buf_expected + (*datafield_len) * 2, datafield_len);
			*buf_expected_len = *datafield_len * 2;

			// DUT
			auto myobj = bbProcessor();
			myobj.work(buf_original, buf_original_len, buf_processed, buf_processed_len, false);

			// turn them into vector
			std::vector<char> vec_original, vec_processed, vec_expected;
			vec_expected.assign(buf_expected, buf_expected + *buf_expected_len);
			vec_processed.assign(buf_processed, buf_processed + *buf_processed_len);
			Assert::IsTrue(vec_expected == vec_processed);
		}

		// what heppens when BB Frame has some garbage data in the end?
		TEST_METHOD(bb_padding_back)
		{
			// varialble declarations
			char* buf_original;
			int* buf_original_len;
			char* buf_processed;
			int* buf_processed_len;
			char* buf_expected;
			int* buf_expected_len;
			int* datafield_len;
			uint8_t* bbFrameID;

			// dersired state
			bbFrameID = new uint8_t;
			*bbFrameID = 2;
			datafield_len = new int;
			*datafield_len = 8;

			buf_original_len = new int;
			*buf_original_len = (*datafield_len + 14) * 3;

			// malloc
			buf_original = new char[*buf_original_len];
			buf_processed = new char[*buf_original_len];
			buf_processed_len = new int;
			buf_expected = new char[*buf_original_len];
			buf_expected_len = new int;

			// create the input
			bbFrameInit(buf_original, datafield_len, bbFrameID);
			bbFrameInit(buf_original + *datafield_len + 14, datafield_len, bbFrameID);
			//bbFrameInit(buf_original + (*datafield_len + 14) * 2, datafield_len, bbFrameID);

			// create expected buffer
			expectedBufInit(buf_expected, datafield_len);
			expectedBufInit(buf_expected + *datafield_len, datafield_len);
			//expectedBufInit(buf_expected + (*datafield_len) * 2, datafield_len);
			*buf_expected_len = *datafield_len * 2;

			// DUT
			auto myobj = bbProcessor();
			myobj.work(buf_original, buf_original_len, buf_processed, buf_processed_len, false);

			// turn them into vector
			std::vector<char> vec_original, vec_processed, vec_expected;
			vec_expected.assign(buf_expected, buf_expected + *buf_expected_len);
			vec_processed.assign(buf_processed, buf_processed + *buf_processed_len);
			Assert::IsTrue(vec_expected == vec_processed);
		}

		// What happens when BB Frame has some garbage in the middle? 
		TEST_METHOD(bb_padding_middle)
		{
			// varialble declarations
			char* buf_original;
			int* buf_original_len;
			char* buf_processed;
			int* buf_processed_len;
			char* buf_expected;
			int* buf_expected_len;
			int* datafield_len;
			uint8_t* bbFrameID;

			// dersired state
			bbFrameID = new uint8_t;
			*bbFrameID = 2;
			datafield_len = new int;
			*datafield_len = 8;

			buf_original_len = new int;
			*buf_original_len = (*datafield_len + 14) * 3;

			// malloc
			buf_original = new char[*buf_original_len];
			buf_processed = new char[*buf_original_len];
			buf_processed_len = new int;
			buf_expected = new char[*buf_original_len];
			buf_expected_len = new int;

			// create the input
			bbFrameInit(buf_original, datafield_len, bbFrameID);
			//bbFrameInit(buf_original + *datafield_len + 14, datafield_len, bbFrameID);
			bbFrameInit(buf_original + (*datafield_len + 14) * 2, datafield_len, bbFrameID);

			// create expected buffer
			expectedBufInit(buf_expected, datafield_len);
			expectedBufInit(buf_expected + *datafield_len, datafield_len);
			//expectedBufInit(buf_expected + (*datafield_len) * 2, datafield_len);
			*buf_expected_len = *datafield_len * 2;

			// DUT
			auto myobj = bbProcessor();
			myobj.work(buf_original, buf_original_len, buf_processed, buf_processed_len, false);

			// turn them into vector
			std::vector<char> vec_original, vec_processed, vec_expected;
			vec_expected.assign(buf_expected, buf_expected + *buf_expected_len);
			vec_processed.assign(buf_processed, buf_processed + *buf_processed_len);
			Assert::IsTrue(vec_expected == vec_processed);
		}

		// What happens when there is communication stops abruptly. and doesn't recover?
		TEST_METHOD(bb_no_recovery)
		{
			// varialble declarations
			char* buf_original;
			int* buf_original_len;
			char* buf_processed;
			int* buf_processed_len;
			char* buf_expected;
			int* buf_expected_len;
			int* datafield_len;
			uint8_t* bbFrameID;

			// dersired state
			bbFrameID = new uint8_t;
			*bbFrameID = 2;
			datafield_len = new int;
			*datafield_len = 8;

			buf_original_len = new int;
			*buf_original_len = ((*datafield_len + 14) * 3) - 4;

			// malloc
			buf_original = new char[*buf_original_len];
			buf_processed = new char[*buf_original_len];
			buf_processed_len = new int;
			buf_expected = new char[*buf_original_len];
			buf_expected_len = new int;

			// create the input
			bbFrameInit(buf_original, datafield_len, bbFrameID);
			bbFrameInit(buf_original + *datafield_len + 14, datafield_len, bbFrameID);
			buf_original[(*datafield_len + 14) * 2] = (unsigned char)0xb8;
			uint16_t datafield_len_bits = (*datafield_len) * 8;
			buf_original[((*datafield_len + 14) * 2) + 9] = (uint8_t)datafield_len_bits & 0x00ff;
			buf_original[((*datafield_len + 14) * 2) + 8] = (uint8_t)((datafield_len_bits >> 8) & 0x00ff);

			for (int i = 14; i < *datafield_len + 10; i++)
			{
				buf_original[((*datafield_len + 14) * 2) + i] = i;
			}

			// create expected buffer
			expectedBufInit(buf_expected, datafield_len);
			expectedBufInit(buf_expected + *datafield_len, datafield_len);
			expectedBufInit(buf_expected + (*datafield_len) * 2, datafield_len);
			*buf_expected_len = *datafield_len * 2;

			// DUT
			int* input_len;
			input_len = new int(*buf_original_len - 4);
			auto myobj = bbProcessor();
			myobj.work(buf_original, input_len, buf_processed, buf_processed_len, false);

			// turn them into vector
			std::vector<char> vec_original, vec_processed, vec_expected;
			vec_expected.assign(buf_expected, buf_expected + *buf_expected_len);
			vec_processed.assign(buf_processed, buf_processed + *buf_processed_len);
			Assert::IsTrue(vec_expected == vec_processed);
		}

		// What happens when there is communicaiton drop and eventually recovers? 
		TEST_METHOD(bb_recovery)
		{
			// varialble declarations
			char* buf_original;
			int* buf_original_len;
			char* buf_processed;
			int* buf_processed_len;
			char* buf_expected;
			int* buf_expected_len;
			int* datafield_len;
			uint8_t* bbFrameID;

			// dersired state
			bbFrameID = new uint8_t;
			*bbFrameID = 2;
			datafield_len = new int;
			*datafield_len = 8;

			buf_original_len = new int;
			*buf_original_len = (*datafield_len + 14) * 3;

			// malloc
			buf_original = new char[*buf_original_len];
			buf_processed = new char[*buf_original_len];
			buf_processed_len = new int;
			buf_expected = new char[*buf_original_len];
			buf_expected_len = new int;

			// create the input
			bbFrameInit(buf_original, datafield_len, bbFrameID);
			bbFrameInit(buf_original + *datafield_len + 14, datafield_len, bbFrameID);
			bbFrameInit(buf_original + ((*datafield_len + 14) * 2) - 1, datafield_len, bbFrameID);

			// create expected buffer
			expectedBufInit(buf_expected, datafield_len);
			expectedBufInit(buf_expected + *datafield_len, datafield_len);
			expectedBufInit(buf_expected + ((*datafield_len) * 2) - 1, datafield_len);
			*buf_expected_len = (*datafield_len * 3) - 1;

			// DUT
			auto myobj = bbProcessor();
			myobj.work(buf_original, buf_original_len, buf_processed, buf_processed_len, false);

			// turn them into vector
			std::vector<char> vec_original, vec_processed, vec_expected;
			vec_expected.assign(buf_expected, buf_expected + *buf_expected_len);
			vec_processed.assign(buf_processed, buf_processed + *buf_processed_len);
			Assert::IsTrue(vec_expected == vec_processed);
		}

	}
	;
}