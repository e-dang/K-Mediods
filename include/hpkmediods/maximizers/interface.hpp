#pragma once

#include <hpkmediods/types/selected_set.hpp>
#include <matrix/matrix.hpp>

namespace hpkmediods
{
template <typename T>
class IMaximizer
{
public:
    virtual ~IMaximizer() = default;

    virtual T maximize(const Matrix<T>* const data, Matrix<T>* const centroids, std::vector<int32_t>* const assignments,
                       Matrix<T>* const dataDistMat, Matrix<T>* const centroidDistMat,
                       SelectedSet* const selectedSet) const = 0;
};
}  // namespace hpkmediods