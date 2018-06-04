#ifndef __DNS_H__
#define __DNS_H__

#ifdef __cplusplus
extern "C"
{
#endif

int gethostbydomain(const char *domain, char *ipAddrBuf, int size);
    
#ifdef __cplusplus
}
#endif

#endif
