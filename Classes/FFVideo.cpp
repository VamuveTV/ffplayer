#include "ff.h"
#include "ffdepends.h"

namespace ff
{
	static bool isInitFF = false;

	FFVideo::FFVideo() :_ctx(nullptr)
	{
		if (!isInitFF){
			initFF();
			isInitFF = true;
		}
	}

	FFVideo::~FFVideo()
	{
		close();
	}

	bool FFVideo::open(const char *url)
	{
		close();
		_ctx = stream_open(url, NULL);
		return isOpen();
	}

	void FFVideo::seek(double t)
	{
		VideoState* _vs = (VideoState*)_ctx;
		if (_vs)
		{
			if (t > length())
				t = length();
			if (t < 0)
				t = 0;
			int64_t ts = t * 1000000LL;
			stream_seek(_vs, ts, 0, 0);
		}
	}

	bool FFVideo::isEnd() const
	{
		VideoState* is = (VideoState*)_ctx;
		if (is)
		{
			if((!is->audio_st || (is->auddec.finished == is->audioq.serial && frame_queue_nb_remaining(&is->sampq) == 0)) &&
				(!is->video_st || (is->viddec.finished == is->videoq.serial && frame_queue_nb_remaining(&is->pictq) == 0))) 
				return true;
		}
		return false;
	}

	double FFVideo::preload() const
	{
		return -1;
	}

	int FFVideo::audio_preload() const
	{
		VideoState* is = (VideoState*)_ctx;
		if (is)
		{
			return frame_queue_nb_remaining(&is->sampq);
		}
		return -1;
	}

	int FFVideo::video_preload() const
	{
		VideoState* is = (VideoState*)_ctx;
		if (is)
		{
			return frame_queue_nb_remaining(&is->pictq);
		}
		return -1;
	}

	double FFVideo::cur() const
	{
		VideoState* _vs = (VideoState*)_ctx;
		if (_vs)
		{
			double pos;
			pos = get_master_clock(_vs);
			if (isnan(pos))
				pos = (double)_vs->seek_pos / AV_TIME_BASE;
			return pos;
		}
		return -1;
	}

	double FFVideo::length() const
	{
		VideoState* _vs = (VideoState*)_ctx;
		if (_vs)
		{
			return (double)_vs->ic->duration / 1000000LL;
		}
		return 0;
	}

	bool FFVideo::hasVideo() const
	{
		VideoState* _vs = (VideoState*)_ctx;
		if (_vs)
		{
			return _vs->video_st ? true : false;
		}
		return false;
	}

	bool FFVideo::hasAudio() const
	{
		VideoState* _vs = (VideoState*)_ctx;
		if (_vs)
		{
			return _vs->audio_st ? true : false;
		}
		return false;
	}

	bool FFVideo::isPause() const
	{
		return isOpen() && is_stream_pause((VideoState*)_ctx);
	}

	bool FFVideo::isPlaying() const
	{
		return !isPause();
	}

	bool FFVideo::isOpen() const
	{
		VideoState* _vs = (VideoState*)_ctx;
		return  (_vs && (_vs->audio_st || _vs->video_st));
	}

	int FFVideo::width() const
	{
		if (!isOpen())return -1;
		VideoState* _vs = (VideoState*)_ctx;
		if (!_vs->video_st->codec)return -1;
		return _vs->video_st->codec->width;
	}

	int FFVideo::height() const
	{
		if (!isOpen())return -1;
		VideoState* _vs = (VideoState*)_ctx;
		if (!_vs->video_st->codec)return -1;
		return _vs->video_st->codec->height;
	}

	void *FFVideo::refresh() const
	{
		VideoState* _vs = (VideoState*)_ctx;
		if (_vs && _vs->pscreen)
		{
			double r = 1 / 30;
			video_refresh(_vs, &r);
			return _vs->pscreen->pixels;
		}
		return nullptr;
	}

	void FFVideo::pause()
	{
		if (isOpen() && !is_stream_pause((VideoState*)_ctx))
		{
			toggle_pause((VideoState*)_ctx);
		}
	}

	void FFVideo::play()
	{
		if (isOpen() && is_stream_pause((VideoState*)_ctx))
		{
			toggle_pause((VideoState*)_ctx);
		}
	}

	void FFVideo::close()
	{
		if (_ctx)
		{
			stream_close((VideoState*)_ctx);
			_ctx = nullptr;
		}
	}
}