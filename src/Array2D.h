#ifndef INC_Array2D_H
#define INC_Array2D_H

//////////////////////////////////////////////////////////////////////////

#include <QtCore/QVector>
#include <QtCore/QSize>


//////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

template<typename Data>
class Array2D : private QVector<Data>
{
public:

    Array2D(const QSize& dimensions);
    Array2D(int width, int height);
    ~Array2D() = default;

    inline int width() const;

    inline int height() const;

    inline QSize size() const;

    inline const Data& get(int x, int y) const;

    inline void set(int x, int y, const Data& data);

    void resize(const QSize& dimensions);

    inline const Data& operator()(int x, int y) const;

    inline Data& operator()(int x, int y);

    inline const Data* rowPtr(int y) const;

    inline Data* rowPtr(int y);

    Array2D<Data> copyRect(int left, int top, int right, int bottom) const;

    bool isInBounds(int x, int y) const;

protected:

    Array2D(const QVector<Data>& data, int width);

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
Array2D<Data>::Array2D(const QVector<Data>& data, int width) :
    QVector<Data>(data),
    mDims(width, data.size() / width)
{
    Q_ASSERT(data.size() % width == 0);
}

template<typename Data>
void Array2D<Data>::resizeData()
{
    QVector<Data>::resize(mDims.width() * mDims.height());
}

template<typename Data>
inline const Data& Array2D<Data>::get(int x, int y) const
{
    Q_ASSERT(isInBounds(x, y));
    return operator()(x, y);
}

template<typename Data>
inline void Array2D<Data>::set(int x, int y, const Data& data)
{
    Q_ASSERT(isInBounds(x, y));
    operator()(x, y) = data;
}

template<typename Data>
inline void Array2D<Data>::resize(const QSize& dimensions)
{
    Q_ASSERT(dimensions.width() >= 0 && dimensions.height() >= 0);
    mDims = dimensions;
    resizeData();
}

template<typename Data>
inline QSize Array2D<Data>::size() const
{
    return mDims;
}

template<typename Data>
inline const Data& Array2D<Data>::operator()(int x, int y) const
{
    return operator[](width() * y + x);
}

template<typename Data>
inline Data& Array2D<Data>::operator()(int x, int y)
{
    return operator[](width() * y + x);
}

template<typename Data>
inline const Data* Array2D<Data>::rowPtr(int y) const
{
    Q_ASSERT(isInBounds(0, y));

    return QVector<Data>::constData() + y * width();
}

template<typename Data>
inline Data* Array2D<Data>::rowPtr(int y)
{
    return QVector<Data>::data() + y * width();
}

template<typename Data>
inline Array2D<Data> Array2D<Data>::copyRect(int left, int top, int right, int bottom) const
{
    Q_ASSERT(left >= 0);
    Q_ASSERT(top >= 0);
    Q_ASSERT(right >= left);
    Q_ASSERT(bottom >= top);
    Q_ASSERT(right < width());
    Q_ASSERT(bottom < height());

    int w = right - left + 1;
    int h = bottom - top + 1;

    /// Copy the relevant part of each row in a QVector
    QVector<Data> copiedCells;
    copiedCells.reserve(w * h);

    for (int row = top; row <= bottom; row++)
    {
        copiedCells.append(mid(row * width() + left, w));
    }

    /// Initialize Array2D with the QVector
    return Array2D<Data>(copiedCells, w);
}

template<typename Data>
inline bool Array2D<Data>::isInBounds(int x, int y) const
{
    return x >= 0 && y >= 0 && x < width() && y < height();
}

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // INC_Array2D_H
