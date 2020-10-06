#ifndef ___UTIL_H__
#define ___UTIL_H__

#include <vector>
#include <iostream>
#include <cstring>
#include <string>
#include <iomanip>
#include <time.h>
#include <utility>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <limits.h>
#include <sys/time.h>
#include <cmath>


using namespace std;

/************************************************************************/
/* DEBUG                                                                */
/************************************************************************/
#define _DEBUG

//#define IMPRIMIR_INFO

/************************************************************************/
/* FUNCOES AUXILIARES                                                   */
/************************************************************************/

#define INF LONG_MAX

#define ZERO 0.00000001
#define DOUBLE_EQUALS(X, Y) (fabs((X)-(Y)) < ZERO)

#define DOUBLE_TO_INT(X) ( (int) ((X) + ZERO) )
#define MIN(X,Y) ((X)<(Y) ? (X) : (Y))
#define MAX(X,Y) ((X)>(Y) ? (X) : (Y))

#define IGUAL(X, Y) fabs((X)-(Y))<ZERO

#ifdef _DEBUG
#define AS(x, i) x.at(i)
#else
#define AS(x, i) x[i]
#endif

//#ifdef __APPLE__
#ifdef _WIN32
#define PAUSE system("pause");
#else
#define PAUSE printf("Pressione uma tecla para continuar...\n"); getchar();
#endif

typedef long long ll;

#define ASSERT(x) \
                 if (! (x)) \
                { \
                  cerr << "ERROR!! Assert " << #x << " failed\n"; \
                  cerr << " on line " << __LINE__  << "\n"; \
                  cerr << " in file " << __FILE__ << "\n";  \
				  PAUSE \
				  exit(0);	\
                }

 /************************************************************************/
/* CLASSE PARA IMPRIMIR VIRGULA EM NUMEROS DECIMAIS                      */
/************************************************************************/
template<typename CharT>
class DecimalSeparator : public std::numpunct<CharT>
{
public:
	DecimalSeparator(CharT Separator)
		: m_Separator(Separator)
	{}

protected:
	CharT do_decimal_point()const
	{
		return m_Separator;
	}

private:
	CharT m_Separator;
};

template<class InIt>
void print_range(InIt first, InIt last, char const* delim = "\n"){
    --last;
    for(; first != last; ++first){
        std::cout << *first << delim;
    }
    std::cout << *first;
};

class MyError : public logic_error {
public:
  string errorMsg;
  explicit MyError(const string& msg = "") : logic_error(msg), errorMsg(msg) {}
  virtual const char * what() const throw() {
    return errorMsg.c_str();
  }
  ~MyError() throw() {}
};


#if defined (MSVC)
#define INLINE __inline
#else
#define INLINE inline
#endif

INLINE double MyClock()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000) + tv.tv_usec;
};


#endif
