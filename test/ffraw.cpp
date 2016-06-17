#include "ffraw.h"
#include "ffenc.h"


static void ffFreeRaw(AVRaw * praw)
{
	if (praw)
	{
		/*
		* �������data��洢��ָ����ͨ��malloc����������ڴ棬����praw->data[0]��ͷ��
		*/
		if (praw->data[0])
			av_free(praw->data[0]);
		free(praw);
	}
}

/*
* ����ͼ�����Ƶ����
*/
AVRaw *make_image_raw(int format, int w, int h)
{
	AVRaw * praw = (AVRaw*)malloc(sizeof(AVRaw));

	while (praw)
	{
		int size;
		memset(praw, 0, sizeof(AVRaw));
		praw->type = RAW_IMAGE;
		praw->format = format;
		praw->width = w;
		praw->height = h;

		int ret = av_image_alloc(praw->data, praw->linesize, w, h, (AVPixelFormat)format, 32);
		if (ret < 0)
		{
			char errmsg[ERROR_BUFFER_SIZE];
			av_strerror(ret, errmsg, ERROR_BUFFER_SIZE);
			ffLog("make_image_raw av_image_alloc : %s \n", errmsg);
			break;
		}
		praw->size = ret;
		return praw;
	}

	/*
	* ��������
	*/
	if (praw)
	{
		ffFreeRaw(praw);
	}
	else
	{
		ffLog("make_image_raw out of memory.\n");
	}
	praw = NULL;
	return praw;
}

AVRaw *make_audio_raw(int format, int channel, int samples)
{
	AVRaw * praw = (AVRaw*)malloc(sizeof(AVRaw));
	while (praw)
	{
		memset(praw, 0, sizeof(AVRaw));
		praw->type = RAW_AUDIO;
		praw->format = format;
		praw->channels = channel;
		praw->samples = samples;

		int ret = av_samples_alloc(praw->data, praw->linesize, channel, samples, (AVSampleFormat)format, 0);
		if (ret > 0)
		{
			praw->size = ret;
		}
		else
		{
			char errmsg[ERROR_BUFFER_SIZE];
			av_strerror(ret, errmsg, ERROR_BUFFER_SIZE);
			ffLog("make_audio_raw av_samples_alloc : %s \n", errmsg);
			break;
		}

		return praw;
	}

	/*
	* ��������
	*/
	if (praw)
	{
		ffFreeRaw(praw);
	}
	else
	{
		ffLog("make_audio_raw out of memory.\n");
	}
	praw = NULL;
	return praw;
}

/*
* raw���ݵ��ͷŻ���ʹ�����û���
* ���ü���<=0��ִ���������ͷŲ���,make������raw�������ü���=0
*/
int retain_raw(AVRaw * praw)
{
	praw->ref++;
	return praw->ref;
}

int release_raw(AVRaw * praw)
{
	if (praw)
	{
		if (praw->ref <= 0)
		{
			ffFreeRaw(praw);
			return -1;
		}
		praw->ref--;
		return praw->ref;
	}
	else
		return -1;
}