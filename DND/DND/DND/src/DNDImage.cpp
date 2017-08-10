#include "DNDImage.h"
#include "DNDDebug.h"
#include "DNDGame.h"
#include "DNDSystem_imp.h"
#include <png.h>

namespace DND
{
	
	Image* Image::Create(Size size, Color color)
	{
		Image* img = new Image;
		img->m_size = size;
		img->m_buffer = new DWORD[(unsigned)size.w*(unsigned)size.h];

		for (unsigned j = 0; j < size.w*size.h; j++)
			img->m_buffer[j] = color.Get();

		return img;
	}

	Image::~Image()
	{
		if (m_buffer)
			delete[] m_buffer;
	}

	Size Image::GetSize() const
	{
		return m_size;
	}

	const DWORD* Image::GetBuffer() const
	{
		return m_buffer;
	}

	void Image::AddImage(const Image* img, const Point& tar_xy)
	{
		AddImageRect(img, Rect(XYWH(Point(), img->GetSize())), tar_xy);
	}

	void Image::AddImageRect(const Image* img, const Rect& rect, const Point& tar_xy)
	{
		if (!img || !img->m_buffer)
		{
			debug_warn(L"DND: Image::AddImageRect�����˿�ָ�룡");
			return;
		}
		Size s_size = rect.Get_Size();

		if ((tar_xy.x + s_size.w > m_size.w) ||
			(tar_xy.y + s_size.h > m_size.h))
		{
			debug_warn(L"DND: Image::AddImageRectԴͼ���С��");
			return;
		}

		unsigned m;
		unsigned n = 0;
		//copy
		for (int j = rect.p1.y; j < rect.p2.y; ++j, ++n)
		{
			m = 0;
			for (int i = rect.p1.x; i < rect.p2.x; ++i, ++m)
			{
				m_buffer[(tar_xy.y + n) * m_size.w + tar_xy.x + m] = img->m_buffer[j * s_size.w + i];
			}

		}
			

	}

	void Image::Render(Point pos)
	{
		System* sys = Game::Get()->sys;
	
		for (UINT32 j = 0; j < m_size.h; ++j)
		{
			for (UINT32 i = 0; i < m_size.w; ++i)
			{
				sys->RenderDot(Vector2((float)(pos.x + i), (float)(pos.y + j)),
					GetDotColor(Point(i, j)));
			}
		}
	}

	Color Image::GetDotColor(Point xy)
	{
		if ((UINT32)xy.x >= m_size.w || (UINT32)xy.y >= m_size.h)
		{
			debug_warn(L"Image Get_Color ��������Խ�磡");
			return Color::WHITE;
		}

		return Color(m_buffer[xy.x + xy.y*m_size.w]);
	}

	void Image::SaveToPNG(const String& path)
	{
		FILE* fp;
		
		png_infop info_ptr;
		//png_colorp

		char cpath[MAX_PATH] = { NULL };
		path.GetMultiByteStr(cpath, MAX_PATH);

		fopen_s(&fp, cpath, "wb");
		if (fp == NULL)
		{
			assert(0 && L"Save_To_PNG �����ļ�ʧ�ܣ�");
			return;
		}

		//��ʼ��pnglib 
		static png_structp png_ptr = NULL;
		if (!png_ptr)
			png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
				NULL, NULL, NULL);
		if (!png_ptr)
		{
			assert(0 && L"Save_To_PNG �����ļ�ʱ��ʼ��pnglibʧ�ܣ�");
			return;
		}

		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL)
		{
			fclose(fp);
			assert(0 && L"Save_To_PNG��png_create_info_structʧ�ܣ�");
			return;
		}

		//������
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			fclose(fp);
			png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
			assert(0 && L"Save_To_PNG��pnglib ���ִ���");
			return;
		}

		unsigned bit_depth = 8;
		unsigned pixel_byte = 4;
		unsigned row_byte = m_size.w * pixel_byte;

		//�����������
		png_init_io(png_ptr, fp);
		

		//����ͼ������
		png_set_IHDR(png_ptr, info_ptr, m_size.w, m_size.h, bit_depth,
			PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, //������
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		//дͷ��
		png_write_info(png_ptr, info_ptr);

		//��ȡ��ָ��
		png_bytepp row_pointers = (png_bytep*)malloc(m_size.h*sizeof(png_bytep));

		
		for (unsigned x = 0; x < m_size.h; ++x)
		{//����һ��
			row_pointers[x] = (png_bytep)malloc(row_byte);
			for (unsigned y = 0; y < row_byte; y += pixel_byte)
			{
				row_pointers[x][y + 2] = ((unsigned char*)m_buffer)[x * row_byte + y + 0];
				row_pointers[x][y + 1] = ((unsigned char*)m_buffer)[x * row_byte + y + 1];
				row_pointers[x][y + 0] = ((unsigned char*)m_buffer)[x * row_byte + y + 2];
				row_pointers[x][y + 3] = ((unsigned char*)m_buffer)[x * row_byte + y + 3];
				/*row_pointers[x][y + 2] =
					row_pointers[x][y + 1] =
					row_pointers[x][y + 0] =
					row_pointers[x][y + 3] = 0xff;*/
				
			}
		}
		//д��ȫ��
		png_write_image(png_ptr, row_pointers);
		//дβ��
		png_write_end(png_ptr, info_ptr);

		//�ͷ�png�ڴ�
		png_destroy_write_struct(&png_ptr, (png_infopp) NULL);

	/*	delete[] row_pointers;
		delete[] image;*/
		for (unsigned x = 0; x < m_size.h; ++x)
		{//�ͷ�ÿ��
			free(row_pointers[x]);
		}
		free(row_pointers);
		fclose(fp);
	}

	//class ImageSource
	//{
	//public:
	//	unsigned char* buffer;
	//	unsigned size;
	//	unsigned offset;
	//	ImageSource() { ZeroMemory(this, sizeof(ImageSource)); }
	//};
	////���ڴ��ȡPNGͼƬ�Ļص�������copy�� csdn��
	//void __cdecl _png_read_callback(png_structp png_ptr, png_bytep data, png_size_t length)
	//{
	//	ImageSource* img = (ImageSource*)png_get_io_ptr(png_ptr);
	//	if (img->offset + length <= img->size)
	//	{
	//		memcpy(data, img->buffer + img->offset, length);
	//		img->offset += length;
	//	}
	//	else
	//		png_error(png_ptr, "pngReaderCallback failed");

	//}


	Image* Image::Create(const String& path)
	{
		
		Image* img = new Image;
		System_imp* sys = (System_imp*)(Game::Get()->sys);
		char temp[512] = { NULL };
		path.GetMultiByteStr(temp, 512);


		///////////////////////////////���ļ�����/////////////////////////////////////
		FILE* fp = NULL;
		fopen_s(&fp, temp, "rb");
		if (!fp)
		{
			debug_warn(String(L"DND: �ļ���ʧ��: ") + path);
			return NULL;
		}
		//�ж��Ƿ��� png �ļ�
		size_t number = 8;
		png_bytep header = new png_byte[number];
		fread(header, 1, number, fp);
		bool is_png = !png_sig_cmp(header, 0, number);
		if (!is_png)
		{
			fclose(fp);
			debug_err(String(L"DND: �ļ���ʽ����: ") + path);
			return NULL;
		}
		//��ʼ��pnglib 
		static png_structp png_ptr = NULL;
		if (!png_ptr)
			png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
				NULL, NULL, NULL);
		if (!png_ptr)
		{
			fclose(fp);
			debug_err(L"DND: ����Imageʱ��ʼ��pnglibʧ�ܣ�");
			return NULL;
		}
		//����ͼ����Ϣ info
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			fclose(fp);
			debug_err(0 && L"DND: ����Imageʱ����png_infoʧ�ܣ�");
			return NULL;
		}
		//������
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			fclose(fp);
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			debug_err(0 && L"DND: ����Imageʱpnglib���ִ���");
			return NULL;
		}
		//��������Դ
		png_init_io(png_ptr, fp);
		//�����ļ�ͷ�Ѵ���
		png_set_sig_bytes(png_ptr, 8);

		//��png
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
			
		//��info��ѯ����
		unsigned w = png_get_image_width(png_ptr, info_ptr);        //���ͼƬ����
		unsigned h = png_get_image_height(png_ptr, info_ptr);        //���ͼƬ�߶�
		int color_type = png_get_color_type(png_ptr, info_ptr);        //���ͼƬ��ɫ

																		//��ֵimage
		img->m_size.w = w;
		img->m_size.h = h;
		img->m_buffer = new DWORD[w*h];

		//��info ���Ƶ� image
		png_bytep *row_point = NULL;
		row_point = png_get_rows(png_ptr, info_ptr);

		int block_size = (color_type == 6 ? 4 : 3);

		//(A)RGB

		unsigned pos = 0;
		for (unsigned x = 0; x < h; ++x)
			for (unsigned y = 0; y < w*block_size; y += block_size)
			{
				((unsigned char*)img->m_buffer)[pos + 0] = row_point[x][y + 2];//b;
				((unsigned char*)img->m_buffer)[pos + 1] = row_point[x][y + 1];//g
				((unsigned char*)img->m_buffer)[pos + 2] = row_point[x][y + 0];//r
				if (color_type == 6)
					((unsigned char*)img->m_buffer)[pos + 3] = row_point[x][y + 3];//a
				else
					((unsigned char*)img->m_buffer)[pos + 3] = 0xff;
				pos += 4;
			}

		fclose(fp);

		//�ͷ�png�ڴ�
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

			
		return img;
		
	}


	Image* Image::Create(const Image* b)
	{
		Image* img = new Image;
		img->m_size = b->m_size;
		UINT32 len = img->m_size.w * img->m_size.h;
		img->m_buffer = new DWORD[len];
		memcpy_s(img->m_buffer, len*sizeof(DWORD), b->m_buffer, len*sizeof(DWORD));

		return img;
	}

	Image::Image() : m_buffer(0)
	{

	}

	Image* Image::Create(void* texture, Size size)
	{
		//Image* img = new Image;
		return NULL;
	}

}
