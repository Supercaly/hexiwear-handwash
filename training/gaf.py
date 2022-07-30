"""Code for Piecewise Aggregate Approximation."""

# Author: Johann Faouzi <johann.faouzi@gmail.com>
# License: BSD-3-Clause

import itertools
import numpy as np
from math import ceil
from sklearn.base import BaseEstimator, TransformerMixin
from sklearn.utils.validation import check_array
from sklearn.preprocessing import MinMaxScaler as SklearnMinMaxScaler
from pyts.preprocessing import MinMaxScaler
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import ImageGrid


def _paa(X, n_samples, n_timestamps, start, end, n_timestamps_new):
    X_paa = np.empty((n_samples, n_timestamps_new))
    for i in range(n_samples):
        for j in range(n_timestamps_new):
            X_paa[i, j] = np.mean(X[i, start[j]:end[j]])
    return X_paa


class PiecewiseAggregateApproximation(BaseEstimator, TransformerMixin):
    """Piecewise Aggregate Approximation.

    Parameters
    ----------
    window_size : int, float or None (default = 1)
        Length of the sliding window. If float, it represents
        a percentage of the size of each time series and must be
        between 0 and 1.

    output_size : int, float or None (default = None)
        Size of the returned time series. If float, it represents
        a percentage of the size of each time series and must be
        between 0. and 1. Ignored if ``window_size`` is not None.
        It can't be None if ``window_size`` is None. If you want to use
        ``output_size`` over ``window_size``, you must set
        ``window_size=None``.

    overlapping : bool (default = True)
        When ``window_size=None``, ``output_size`` is used to derive the window
        size; the window size is fixed if ``overlapping=True`` and may vary
        if ``overlapping=False``. Ignored if ``window_size`` is specified.

    References
    ----------
    .. [1] E. Keogh, K. Chakrabarti, M. Pazzani, and S. Mehrotra,
           "Dimensionality reduction for fast similarity search in large
           time series databases". Knowledge and information Systems,
           3(3), 263-286 (2001).

    Examples
    --------
    >>> from pyts.approximation import PiecewiseAggregateApproximation
    >>> X = [[0, 4, 2, 1, 7, 6, 3, 5],
    ...      [2, 5, 4, 5, 3, 4, 2, 3]]
    >>> transformer = PiecewiseAggregateApproximation(window_size=2)
    >>> transformer.transform(X)
    array([[2. , 1.5, 6.5, 4. ],
           [3.5, 4.5, 3.5, 2.5]])

    """

    def __init__(self, window_size=1, output_size=None, overlapping=True):
        self.window_size = window_size
        self.output_size = output_size
        self.overlapping = overlapping

    def fit(self, X=None, y=None):
        """Pass.

        Parameters
        ----------
        X
            Ignored
        y
            Ignored

        Returns
        -------
        self
            object

        """
        return self

    def transform(self, X):
        """Reduce the dimensionality of each time series.

        Parameters
        ----------
        X : array-like, shape = (n_samples, n_timestamps)

        Returns
        -------
        X_new : array, shape = (n_samples, n_timestamps_new)

        """
        X = check_array(X)
        n_samples, n_timestamps = X.shape

        window_size, output_size = self._check_params(n_timestamps)
        if window_size == 1:
            return X
        else:
            start, end, n_timestamps_new = segmentation(
                n_timestamps, window_size, self.overlapping, output_size
            )
            X_paa = _paa(X, n_samples, n_timestamps,
                         start, end, n_timestamps_new)
            return X_paa

    def _check_params(self, n_timestamps):
        if (self.window_size is None and self.output_size is None):
            raise TypeError("'window_size' and 'output_size' cannot be "
                            "both None.")
        if self.window_size is not None:
            if not isinstance(self.window_size,
                              (int, np.integer, float, np.floating)):
                raise TypeError("If specified, 'window_size' must be an "
                                "integer or a float.")
            if isinstance(self.window_size, (int, np.integer)):
                if not 1 <= self.window_size <= n_timestamps:
                    raise ValueError(
                        "If 'window_size' is an integer, it must be greater "
                        "than or equal to 1 and lower than or equal to "
                        "n_timestamps (got {0}).".format(self.window_size)
                    )
                window_size = self.window_size
            else:
                if not 0 < self.window_size <= 1:
                    raise ValueError(
                        "If 'window_size' is a float, it must be greater "
                        "than 0 and lower than or equal to 1 "
                        "(got {0}).".format(self.window_size)
                    )
                window_size = ceil(self.window_size * n_timestamps)
            output_size = None
        else:
            if not isinstance(self.output_size,
                              (int, np.integer, float, np.floating)):
                raise TypeError("If specified, 'output_size' must be an "
                                "integer or a float.")
            if isinstance(self.output_size, (int, np.integer)):
                if not 1 <= self.output_size <= n_timestamps:
                    raise ValueError(
                        "If 'output_size' is an integer, it must be greater "
                        "than or equal to 1 and lower than or equal to "
                        "n_timestamps (got {0}).".format(self.output_size)
                    )
                output_size = self.output_size
            else:
                if not 0 < self.output_size <= 1.:
                    raise ValueError(
                        "If 'output_size' is a float, it must be greater "
                        "than 0 and lower than or equal to 1 "
                        "(got {0}).".format(self.output_size)
                    )
                output_size = ceil(self.output_size * n_timestamps)
            window_size, remainder = divmod(n_timestamps, output_size)
            if remainder != 0:
                window_size += 1
        return window_size, output_size


def segmentation(ts_size, window_size, overlapping=False, n_segments=None):
    """Compute the indices for Piecewise Agrgegate Approximation.

    Parameters
    ----------
    ts_size : int
        The size of the time series.

    window_size : int
        The size of the window.

    overlapping : bool (default = False)
        If True, overlapping windows may be used. If False, non-overlapping
        are used.

    n_segments : int or None (default = None)
        The number of windows. If None, the number is automatically
        computed using ``window_size``.

    Returns
    -------
    start : array
        The lower bound for each window.

    end : array
        The upper bound for each window.

    size : int
        The size of ``start``.

    Examples
    --------
    >>> from pyts.utils import segmentation
    >>> start, end, size = segmentation(ts_size=12, window_size=3)
    >>> print(start)
    [0 3 6 9]
    >>> print(end)
    [ 3  6  9 12]
    >>> size
    4

    """
    if not isinstance(ts_size, (int, np.integer)):
        raise TypeError("'ts_size' must be an integer.")
    if not ts_size >= 2:
        raise ValueError("'ts_size' must be an integer greater than or equal "
                         "to 2 (got {0}).".format(ts_size))
    if not isinstance(window_size, (int, np.integer)):
        raise TypeError("'window_size' must be an integer.")
    if not window_size >= 1:
        raise ValueError("'window_size' must be an integer greater than or "
                         "equal to 1 (got {0}).".format(window_size))
    if not window_size <= ts_size:
        raise ValueError("'window_size' must be lower than or equal to "
                         "'ts_size' ({0} > {1}).".format(window_size, ts_size))
    if not (n_segments is None or isinstance(n_segments, (int, np.integer))):
        raise TypeError("'n_segments' must be None or an integer.")
    if isinstance(n_segments, (int, np.integer)):
        if not n_segments >= 2:
            raise ValueError(
                "If 'n_segments' is an integer, it must be greater than or "
                "equal to 2 (got {0}).".format(n_segments)
            )
        if not n_segments <= ts_size:
            raise ValueError(
                "If 'n_segments' is an integer, it must be lower than or "
                "equal to 'ts_size' ({0} > {1}).".format(n_segments, ts_size)
            )

    if n_segments is None:
        quotient, remainder = divmod(ts_size, window_size)
        n_segments = quotient if remainder == 0 else quotient + 1

    if not overlapping:
        bounds = np.linspace(0, ts_size, n_segments + 1).astype('int64')
        start = bounds[:-1]
        end = bounds[1:]
        size = start.size
        return start, end, size
    else:
        n_overlapping = (n_segments * window_size) - ts_size
        n_overlaps = n_segments - 1
        overlaps = np.linspace(0, n_overlapping,
                               n_overlaps + 1).astype('int64')
        bounds = np.arange(0, (n_segments + 1) * window_size, window_size)
        start = bounds[:-1] - overlaps
        end = bounds[1:] - overlaps
        size = start.size
        return start, end, size


#@njit()
def _gasf(X_cos, X_sin, n_samples, image_size):
    X_gasf = np.empty((n_samples, image_size, image_size))
    
    for i in range(n_samples):
        X_gasf[i] = np.outer(X_cos[i], X_cos[i]) - np.outer(X_sin[i], X_sin[i])
    return X_gasf


#@njit()
def _gadf(X_cos, X_sin, n_samples, image_size):
    X_gadf = np.empty((n_samples, image_size, image_size))
    for i in range(n_samples):
        X_gadf[i] = np.outer(X_sin[i], X_cos[i]) - np.outer(X_cos[i], X_sin[i])
    return X_gadf


class GramianAngularField(BaseEstimator, TransformerMixin):
    """Gramian Angular Field.

    Parameters
    ----------
    image_size : int or float (default = 1.)
        Shape of the output images. If float, it represents a percentage
        of the size of each time series and must be between 0 and 1. Output
        images are square, thus providing the size of one dimension is enough.

    sample_range : None or tuple (min, max) (default = (-1, 1))
        Desired range of transformed data. If None, no scaling is performed
        and all the values of the input data must be between -1 and 1.
        If tuple, each sample is scaled between min and max; min must be
        greater than or equal to -1 and max must be lower than or equal to 1.

    method : 'summation' or 'difference' (default = 'summation')
        Type of Gramian Angular Field. 's' can be used for 'summation'
        and 'd' for 'difference'.

    overlapping : bool (default = False)
        If True, reduce the size of each time series using PAA with possible
        overlapping windows.

    flatten : bool (default = False)
        If True, images are flattened to be one-dimensional.

    References
    ----------
    .. [1] Z. Wang and T. Oates, "Encoding Time Series as Images for Visual
           Inspection and Classification Using Tiled Convolutional Neural
           Networks." AAAI Workshop (2015).

    Examples
    --------
    >>> from pyts.datasets import load_gunpoint
    >>> from pyts.image import GramianAngularField
    >>> X, _, _, _ = load_gunpoint(return_X_y=True)
    >>> transformer = GramianAngularField()
    >>> X_new = transformer.transform(X)
    >>> X_new.shape
    (50, 150, 150)

    """

    def __init__(self, image_size=1., sample_range=(-1, 1),
                 method='summation', overlapping=False, flatten=False):
        self.image_size = image_size
        self.sample_range = sample_range
        self.method = method
        self.overlapping = overlapping
        self.flatten = flatten

    def fit(self, X=None, y=None):
        """Pass.

        Parameters
        ----------
        X
            Ignored
        y
            Ignored

        Returns
        -------
        self : object

        """
        return self

    def transform(self, X):
        """Transform each time series into a GAF image.

        Parameters
        ----------
        X : array-like, shape = (n_samples, n_timestamps)

        Returns
        -------
        X_new : array-like, shape = (n_samples, image_size, image_size)
            Transformed data. If ``flatten=True``, the shape is
            `(n_samples, image_size * image_size)`.

        """
        X = check_array(X)
        n_samples, n_timestamps = X.shape
        image_size = self._check_params(n_timestamps)

        paa = PiecewiseAggregateApproximation(
            window_size=None, output_size=image_size,
            overlapping=self.overlapping
        )
        X_paa = paa.fit_transform(X)
        if self.sample_range is None:
            X_min, X_max = np.min(X_paa), np.max(X_paa)
            if (X_min < -1) or (X_max > 1):
                raise ValueError("If 'sample_range' is None, all the values "
                                 "of X must be between -1 and 1.")
            X_cos = X_paa
        else:
            scaler = MinMaxScaler(sample_range=self.sample_range)
            X_cos = scaler.fit_transform(X_paa)
        X_sin = np.sqrt(np.clip(1 - X_cos ** 2, 0, 1))
        if self.method in ['s', 'summation']:
            X_new = _gasf(X_cos, X_sin, n_samples, image_size)
        else:
            X_new = _gadf(X_cos, X_sin, n_samples, image_size)

        if self.flatten:
            return X_new.reshape(n_samples, -1)
        return X_new

    def _check_params(self, n_timestamps):
        if not isinstance(self.image_size,
                          (int, np.integer, float, np.floating)):
            raise TypeError("'image_size' must be an integer or a float.")
        if isinstance(self.image_size, (int, np.integer)):
            if self.image_size < 1 or self.image_size > n_timestamps:
                raise ValueError(
                    "If 'image_size' is an integer, it must be greater "
                    "than or equal to 1 and lower than or equal to "
                    "n_timestamps ({0}) (got {1}).".format(n_timestamps, self.image_size)
                )
            image_size = self.image_size
        else:
            if not (0 < self.image_size <= 1.):
                raise ValueError(
                    "If 'image_size' is a float, it must be greater "
                    "than 0 and lower than or equal to 1 (got {0})."
                    .format(self.image_size)
                )
            image_size = ceil(self.image_size * n_timestamps)
        if not ((self.sample_range is None)
                or (isinstance(self.sample_range, tuple))):
            raise TypeError("'sample_range' must be None or a tuple.")
        if isinstance(self.sample_range, tuple):
            if len(self.sample_range) != 2:
                raise ValueError("If 'sample_range' is a tuple, its length "
                                 "must be equal to 2.")
            if not -1 <= self.sample_range[0] < self.sample_range[1] <= 1:
                raise ValueError(
                    "If 'sample_range' is a tuple, it must satisfy "
                    "-1 <= sample_range[0] < sample_range[1] <= 1."
                )
        if self.method not in ['s', 'd', 'summation', 'difference']:
            raise ValueError("'method' must be either 'summation', 's', "
                             "'difference' or 'd'.")
        return image_size


if __name__ == "__main__":
    # X = [[0, 4, 2, 1, 7, 6, 3, 5], [2, 5, 4, 5, 3, 4, 2, 3]]
    # X = np.array(X)
    # X = [[-1, 2], [-0.5, 6], [0, 10], [1, 18]]
    # X, _, _, _ = load_gunpoint(return_X_y=True)
    # print(X.shape)
    # data = list(itertools.islice(load_raw_data("all_db.csv"), 400))
    # ax = [float(f["ax"]) for f in data]
    # ax = np.array([ax])
    # ay = [float(f["ay"]) for f in data]
    # ay = np.array([ay])
    # az = [float(f["az"]) for f in data]
    # az = np.array([az])
    # gx = [float(f["gx"]) for f in data]
    # gx = np.array([gx])
    # gy = [float(f["gy"]) for f in data]
    # gy = np.array([gy])
    # gz = [float(f["gz"]) for f in data]
    # gz = np.array([gz])
    
    # print(ax.shape)

    time_points = np.linspace(0, 4 * np.pi, 500)
    x = np.sin(time_points)
    X = np.array([x])

    gasf = GramianAngularField(method='summation')
    gadf = GramianAngularField(method='difference')
    ax_gasf = gasf.fit_transform(X)
    ax_gadf = gadf.fit_transform(X)
    
    print("aa ", ax_gasf.shape)

    fig = plt.figure(figsize=(8, 4))
    grid = ImageGrid(fig, 111,
                 nrows_ncols=(6, 2),
                 axes_pad=0.15,
                 share_all=True,
                 cbar_location="right",
                 cbar_mode="single",
                 cbar_size="7%",
                 cbar_pad=0.3,
                 )
    images = [ax_gasf[0], ax_gadf[0]]
    # , ay_gasf[0], ay_gadf[0], az_gasf[0], az_gadf[0], gx_gasf[0], gx_gadf[0], gy_gasf[0], gy_gadf[0], gz_gasf[0], gz_gadf[0]]
    titles = ['Summation', 'Diff']
    for image, idx, ax in zip(images, range(len(images)), grid):
        im = ax.imshow(image, cmap='rainbow', origin='lower')
        if idx in [0, 1]:
            ax.set_title(titles[idx % 2], fontdict={'fontsize': 12})
    ax.cax.colorbar(im)
    ax.cax.toggle_label(True)
    plt.suptitle('Gramian Angular Fields', y=0.98, fontsize=16)
    plt.show()

