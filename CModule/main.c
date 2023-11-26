#include <stdbool.h>
#include <Python.h>
#include <longobject.h>

#define As_PyObj(x) (PyObject *) x
#define PyObject_Clear(obj)     \
    do                          \
    {                           \
        if (obj) {              \
            Py_DECREF(obj);     \
            obj = NULL;         \
        }                       \
    } while(0)

PyObject* newton_reverse(PyObject* A, PyObject* B, size_t step)
{
    // Objects
    PyObject* tmp1 = NULL; /* Object for temp calcs */
    PyObject* tmp2 = NULL; /* Object for temp calcs */
    PyObject* tmp3 = NULL; /* Object for temp calcs */
    PyObject* N = NULL;
    PyObject* N_len = NULL;
    PyObject* x = NULL;
    PyObject* R = NULL;
    PyObject* p = NULL;
    PyObject* vv = NULL;
    PyObject* shift = NULL;
    PyObject* prod = NULL;

    bool cmp_res = false;
    long t = 1;

    N = _PyLong_Copy((PyLongObject*)B);
    while (true)
    {
        tmp1 = PyNumber_Remainder(N, PyLong_FromLong(2L));
        cmp_res = PyObject_RichCompareBool(tmp1, PyLong_FromLong(0L), Py_EQ);
        PyObject_Clear(tmp1);
        if (!cmp_res)
            break;

        N = PyNumber_InPlaceRshift(N, PyLong_FromLong(1));
    }

    cmp_res = PyObject_RichCompareBool(N, PyLong_FromLong(1L), Py_EQ);
    if (cmp_res)
        return N;

    N_len = PyLong_FromSize_t(_PyLong_NumBits(N) - 1);
    p = PyNumber_Lshift(PyLong_FromLong(4L), N_len);

    tmp1 = PyNumber_Multiply(PyLong_FromLong(3L), N);
    cmp_res = PyObject_RichCompareBool(tmp1, p, Py_GT);
    PyObject_Clear(tmp1);

    x = PyLong_FromLong(cmp_res ? 2L : 3L);


    for (int i = 0; i < step; ++i) {
        PyObject_Clear(prod);
        PyObject_Clear(vv);
        PyObject_Clear(shift);
        PyObject_Clear(R);
        vv = PyLong_FromLong(2 * t + 3);
        shift = PyLong_FromLong(_PyLong_NumBits(N) - (2 * t + 3));
        cmp_res = PyObject_RichCompareBool(shift, PyLong_FromLong(0L), Py_GT);
        if (cmp_res) {
            R = PyNumber_Rshift(N, shift);
            R = PyNumber_InPlaceAdd(R, PyLong_FromLong(1L));
        }
        else {
            tmp1 = PyNumber_Negative(shift);
            R = PyNumber_Lshift(N, tmp1);
            PyObject_Clear(tmp1);
        }

        {   // x *= 2**(1 + vv + t) - x*R
            tmp1 = PyLong_FromLong(t);
            tmp1 = PyNumber_InPlaceAdd(tmp1, vv);
            tmp1 = PyNumber_InPlaceAdd(tmp1, PyLong_FromLong(1L));
            tmp3 = PyNumber_Power(PyLong_FromLong(2L), tmp1, Py_None);
            tmp2 = PyNumber_Multiply(x, R);
            tmp3 = PyNumber_InPlaceSubtract(tmp3, tmp2);
            x = PyNumber_InPlaceMultiply(x, tmp3);
            PyObject_Clear(tmp1);
            PyObject_Clear(tmp2);
            PyObject_Clear(tmp3);
        }

        t *= 2;

        {
            // x >>= x.bit_length() - t - 1
            tmp1 = PyLong_FromLong((long)(_PyLong_NumBits(x) - t - 1));
            x = PyNumber_InPlaceRshift(x, tmp1);
            PyObject_Clear(tmp1);
        }

        {   // prod = (x + 1) * R
            if (prod) PyObject_Clear(prod);
            prod = PyNumber_Add(x, PyLong_FromLong(1L));

            prod = PyNumber_InPlaceMultiply(prod, R);
        }

        {   // p = 1 << (vv + t)
            tmp1 = PyLong_FromLong(t);
            tmp1 = PyNumber_InPlaceAdd(tmp1, vv);
            p = PyNumber_Lshift(PyLong_FromLong(1L), tmp1);
            PyObject_Clear(tmp1);
        }

        while (true)
        {
            cmp_res = PyObject_RichCompareBool(prod, p, Py_LT);
            if (!cmp_res) break;

            x = PyNumber_InPlaceAdd(x, PyLong_FromLong(1L));
            prod = PyNumber_InPlaceAdd(prod, R);
        }
    }
    PyObject_Clear(N_len);

    if ((1 << step) <= (_PyLong_NumBits(B) - 1))
    {
        {   // prod = (x + 1) * N 
            tmp1 = PyNumber_Add(x, PyLong_FromLong(1L));
            prod = PyNumber_Multiply(tmp1, N);
            PyObject_Clear(tmp1);
        }

        p = PyLong_FromLong(1 << ((_PyLong_NumBits(B) - 1) + _PyLong_NumBits(x)));

        while (true)
        {
            cmp_res = PyObject_RichCompareBool(prod, p, Py_LT);
            if (!cmp_res) break;

            prod = PyNumber_InPlaceAdd(prod, N);
            x = PyNumber_InPlaceAdd(x, PyLong_FromLong(1L));
        }

    }

    return x;
}

static PyObject* fast_mod_operation(PyObject* x, PyObject* y)
{
    short small = 4;
    PyObject* r = _PyLong_Copy((PyLongObject*)x);
    PyObject* tmp1 = NULL;
    PyObject* tmp2 = NULL;
    PyObject* rev_y = NULL;
    PyObject* q = NULL;

    bool cmp_res = false;
    size_t n, m, k;
    size_t step = 1;
    size_t p = 3;

    for (int i = 0; i < small; ++i)
    {
        if (PyObject_RichCompareBool((PyObject*)r, (PyObject*)y, Py_LT))
        {
            PyObject* result = PyTuple_New(2);
            PyTuple_SetItem(result, 0, PyLong_FromLong(i));
            PyTuple_SetItem(result, 1, r);
        }
        r = PyNumber_InPlaceSubtract((PyObject*)r, (PyObject*)y);
    }

    PyObject_Clear(r);
    n = _PyLong_NumBits(x);
    m = _PyLong_NumBits(y);
    k = n - m;
    {
        tmp1 = PyLong_FromLong(k);
        tmp2 = PyNumber_Lshift(y, tmp1);
        cmp_res = PyObject_RichCompareBool(x, tmp2, Py_GE);
        if (cmp_res)
            ++k;
        PyObject_Clear(tmp1);
        PyObject_Clear(tmp2);
    }

    while (p <= k)
    {
        ++step;
        p = 2 * p - 1;
    }

    rev_y = newton_reverse(x, y, step);

    q = PyNumber_Multiply(x, rev_y);
    {
        tmp1 = PyLong_FromLong(_PyLong_NumBits(q) - k);
        q = PyNumber_InPlaceRshift(q, tmp1);
        PyObject_Clear(tmp1);
    }

    {
        tmp1 = PyNumber_Multiply(y, q);
        r = PyNumber_Subtract(x, tmp1);
        PyObject_Clear(tmp1);
    }

    while (true) {
        cmp_res = PyObject_RichCompareBool(r, y, Py_GE);
        if (!cmp_res) break;

        q = PyNumber_InPlaceAdd(q, PyLong_FromLong(1L));
        r = PyNumber_InPlaceSubtract(r, y);
    }

    while (true) {
        cmp_res = PyObject_RichCompareBool(r, PyLong_FromLong(0L), Py_LT);
        if (!cmp_res) break;

        q = PyNumber_InPlaceSubtract(q, PyLong_FromLong(1L));
        r = PyNumber_InPlaceAdd(r, y);
    }

    PyObject* result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, q);
    PyTuple_SetItem(result, 1, r);

    return result;
}

static PyObject* method_custom_divmod(PyObject* self, PyObject* args)
{
    PyObject* A = NULL;
    PyObject* B = NULL;
    PyObject* result = NULL;

    if (!PyArg_ParseTuple(args, "OO", &A, &B))
    {
        PyErr_SetString(PyExc_TypeError, "Failed to parse on of numbers");
        return NULL;
    }

    result = fast_mod_operation(A, B);
    return result;
}

static PyMethodDef CustomDivModMethods[] = {
    {"custom_divmod", method_custom_divmod, METH_VARARGS, "Custom divmod function"},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef cutomdivmodmodule = {
    PyModuleDef_HEAD_INIT,
    "custom_divmod_module",
    "Python library with custom divmod",
    -1,
    CustomDivModMethods
};

PyMODINIT_FUNC PyInit_custom_divmod_module(void) {
    return PyModule_Create(&cutomdivmodmodule);
}