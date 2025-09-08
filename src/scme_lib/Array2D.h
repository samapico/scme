#ifndef INC_Array2D_H
#define INC_Array2D_H

//////////////////////////////////////////////////////////////////////////

#include "Global.h"

#include <vector>
#include <QtCore/QSize>


//////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

template<typename Data>
class Array2D : private std::vector<Data>
{
public:

    Array2D(const QSize& dimensions);
    Array2D(int width, int height);
    ~Array2D() = default;

    Array2D<Data>& operator=(const Array2D<Data>& rhs);

    inline int width() const;

    inline int height() const;

    inline QSize size2d() const;

    inline const Data& get(const QPoint& xy) const;

    inline const Data& get(int x, int y) const;

    inline void set(int x, int y, const Data& data);

    inline void set(const QPoint& xy, const Data& data);

    void resize(const QSize& dimensions);

    inline const Data& operator()(int x, int y) const;

    inline Data& operator()(int x, int y);

    inline const Data& operator()(const QPoint& xy) const;

    inline Data& operator()(const QPoint& xy);

    inline const Data* rowPtr(int y) const;

    inline Data* rowPtr(int y);

    //Array2D<Data> copyRect(int left, int top, int right, int bottom) const;

    bool isInBounds(int x, int y) const;

    bool isInBounds(const QPoint& xy) const;

protected:

    Array2D(const std::vector<Data>& data, int width);

    void resizeData();

    QSize mDims;
};

//////////////////////////////////////////////////////////////////////////

template<typename Data>
Array2D<Data>::Array2D(const QSize& dimensions) :
    mDims(dimensions)
{
    resizeData();
}

template<typename Data>
Array2D<Data>::Array2D(int width, int height) :
    Array2D<Data>::Array2D(QSize(width, height))
{
}

template<typename Data>
Array2D<Data>::Array2D(const std::vector<Data>& data, int width) :
    std::vector<Data>(data),
    mDims(width, static_cast<int>(data.size() / width))
{
    Q_ASSERT(data.size() < std::numeric_limits<int>::max());
    Q_ASSERT(static_cast<int>(data.size()) % width == 0);
}

template<typename Data>
Array2D<Data>& Array2D<Data>::operator=(const Array2D<Data>& rhs)
{
    std::vector<Data>::operator=(rhs);
    mDims = rhs.mDims;

    Q_ASSERT(this->rowPtr(0) == this->data());
    Q_ASSERT(this->rowPtr(0) != rhs.rowPtr(0));
    Q_ASSERT(this->data() != rhs.data());

    return *this;
}

template<typename Data>
void Array2D<Data>::resizeData()
{
    std::vector<Data>::resize(mDims.width() * mDims.height());
}

template<typename Data>
inline const Data& Array2D<Data>::get(int x, int y) const
{
    Q_ASSERT(isInBounds(x, y));
    return operator()(x, y);
}

template<typename Data>
inline const Data& Array2D<Data>::get(const QPoint& xy) const
{
    Q_ASSERT(isInBounds(xy));
    return operator()(xy);
}

template<typename Data>
inline void Array2D<Data>::set(int x, int y, const Data& data)
{
    Q_ASSERT(isInBounds(x, y));
    operator()(x, y) = data;
}

template<typename Data>
inline void Array2D<Data>::set(const QPoint& xy, const Data& data)
{
    Q_ASSERT(isInBounds(xy));
    operator()(xy) = data;
}

template<typename Data>
inline void Array2D<Data>::resize(const QSize& dimensions)
{
    Q_ASSERT(dimensions.width() >= 0 && dimensions.height() >= 0);
    mDims = dimensions;
    resizeData();
}

template<typename Data>
inline int Array2D<Data>::width() const
{
    return mDims.width();
}

template<typename Data>
inline int Array2D<Data>::height() const
{
    return mDims.height();
}

template<typename Data>
inline QSize Array2D<Data>::size2d() const
{
    Q_ASSERT(mDims.width() * mDims.height() == this->size());
    return mDims;
}

template<typename Data>
inline const Data& Array2D<Data>::operator()(int x, int y) const
{
    return std::vector<Data>::at(width()* y + x);
}

template<typename Data>
inline const Data& Array2D<Data>::operator()(const QPoint& xy) const
{
    return std::vector<Data>::at(width() * xy.y() + xy.x());
}

template<typename Data>
inline Data& Array2D<Data>::operator()(int x, int y)
{
    return std::vector<Data>::operator[](width() * y + x);
}

template<typename Data>
inline Data& Array2D<Data>::operator()(const QPoint& xy)
{
    return std::vector<Data>::operator[](width()* xy.y() + xy.x());
}

template<typename Data>
inline const Data* Array2D<Data>::rowPtr(int y) const
{
    Q_ASSERT(isInBounds(0, y));

    return std::vector<Data>::data() + y * width();
}

template<typename Data>
inline Data* Array2D<Data>::rowPtr(int y)
{
    return std::vector<Data>::data() + y * width();
}

//template<typename Data>
//inline Array2D<Data> Array2D<Data>::copyRect(int left, int top, int right, int bottom) const
//{
//    Q_ASSERT(left >= 0);
//    Q_ASSERT(top >= 0);
//    Q_ASSERT(right >= left);
//    Q_ASSERT(bottom >= top);
//    Q_ASSERT(right < width());
//    Q_ASSERT(bottom < height());
//
//    int w = right - left + 1;
//    int h = bottom - top + 1;
//
//    /// Copy the relevant part of each row in a QVector
//    std::vector<Data> copiedCells;
//    copiedCells.reserve(w * h);
//
//    for (int row = top; row <= bottom; row++)
//    {
//        copiedCells.append(QVector<Data>::mid(row * width() + left, w));
//    }
//
//    /// Initialize Array2D with the QVector
//    return Array2D<Data>(copiedCells, w);
//}

template<typename Data>
inline bool Array2D<Data>::isInBounds(int x, int y) const
{
    return x >= 0 && y >= 0 && x < width() && y < height();
}

template<typename Data>
inline bool Array2D<Data>::isInBounds(const QPoint& xy) const
{
    return isInBounds(xy.x(), xy.y());
}

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // INC_Array2D_H
