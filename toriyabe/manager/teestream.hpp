// TODO
#include <streambuf>

// TODO
class TeeBuf: public streambuf {
	public:
		TeeBuf(streambuf* sb1, streambuf* sb2);
	protected:
		// This tee buffer has no buffer. So every character "overflows"
		// and can be put directly into the teed buffers.
		virtual int overflow(int c);
		virtual int sync();
	private:
		std::streambuf* sb1;
		std::streambuf* sb2;
};

// TODO
class TeeStream: public ostream {
	ostream& os1;
	ostream& os2;
	public:
	TeeStream(ostream& os1, ostream& os2);
	TeeStream& flush();
	private:
	TeeBuf tbuf;
};

// TODO
class NullBuf: public streambuf {
	public:
		virtual int overflow(int c);
};

// TODO
class NullStream: public ostream {
	public:
		NullStream();
	private:
		NullBuf nullBuf;
};
