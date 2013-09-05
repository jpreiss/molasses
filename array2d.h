#pragma once

#include <utility>

template <typename T, size_t Rows, size_t Columns>
class StaticArray2D
{
public:
	// default-construct all data
	void initialize()
	{
		for (size_t i = 0; i < Rows * Columns; ++i)
		{
			data_[i] = T();
		}
	}

	size_t rows() const
	{
		return Rows;
	}

	size_t columns() const
	{
		return Columns;
	}

	size_t to1D(size_t row, size_t column) const
	{
		return row * Columns + column;
	}

	std::pair<size_t, size_t> to2D(size_t index) const
	{
		return std::make_pair(index / Columns, index % Columns);
	}

	T const &operator[](size_t index) const
	{
		return data_[index];
	}

	T &operator[](size_t index)
	{
		return data_[index];
	}

	T const &operator()(size_t row, size_t column) const
	{
		return data_[to1D(row, column)];
	}

	T &operator()(size_t row, size_t column)
	{
		return data_[to1D(row, column)];
	}

	T const *data() const
	{
		return data_;
	}

	T *data()
	{
		return data_;
	}

	void const *raw() const
	{
		return data_;
	}

	void *raw()
	{
		return data_;
	}

	T const *begin() const
	{
		return data_;
	}

	T *begin()
	{
		return data_;
	}

	T const *end() const
	{
		return data_ + Rows * Columns;
	}

	T *end()
	{
		return data_ + Rows * Columns;
	}

private:
	T data_[Rows * Columns];
};


template <typename T>
class Array2D
{
public:
	Array2D(size_t rows, size_t columns) :
		rows_(rows),
		columns_(columns),
		data_(new T[rows * columns])
	{
	}

	~Array2D()
	{
		delete[] data_;
	}

	// default-construct all data
	void fill(T const &value = T())
	{
		for (size_t i = 0; i < rows_ * columns_; ++i)
		{
			data_[i] = value;
		}
	}

	size_t rows() const
	{
		return rows_;
	}

	size_t columns() const
	{
		return columns_;
	}

	size_t size() const
	{
		return rows() * columns();
	}

	size_t to1D(size_t row, size_t column) const
	{
		return row * columns_ + column;
	}

	std::pair<size_t, size_t> to2D(size_t index) const
	{
		return std::make_pair(index / Columns, index % Columns);
	}

	T const &operator[](size_t index) const
	{
		return data_[index];
	}

	T &operator[](size_t index)
	{
		return data_[index];
	}

	T const &operator()(size_t row, size_t column) const
	{
		return data_[to1D(row, column)];
	}

	T &operator()(size_t row, size_t column)
	{
		return data_[to1D(row, column)];
	}

	T const *data() const
	{
		return data_;
	}

	T *data()
	{
		return data_;
	}
	
	void const *raw() const
	{
		return data_;
	}

	void *raw()
	{
		return data_;
	}

	T const *begin() const
	{
		return data_;
	}

	T *begin()
	{
		return data_;
	}

	T const *end() const
	{
		return data_ + rows_ * columns_;
	}

	T *end()
	{
		return data_ + rows_ * columns_;
	}

private:
	size_t rows_;
	size_t columns_;
	T *data_;
};