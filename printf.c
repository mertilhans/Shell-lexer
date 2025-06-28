#include <unistd.h>
#include <stdarg.h>

int     ft_putchar(char c)
{
        return(write(1,&c,1));

}
int     ft_putstr(char *s)
{
        int len = 0;
         if(!s)
                return(write(1,"null",6));
        while(*s)
                len += write(1,s++,1);
        return(len);
}
int     ft_putnbr(long n ,int base, char *digit)
{
        int res = 0;
        if(n < 0)
        {
                res += write(1,"-",1);
                n = -n;
        }
        if (n >= base)
                res += ft_putnbr(n / base,base,digit);
        return(res + write(1,&digit[n % base],1));
}
int     ft_format(va_list *args,char c)
{
        if(c == 'd')
                return(ft_putnbr(va_arg(*args, int), 10, "0123456789"));
        if(c == 's')
                return(ft_putstr(va_arg(*args ,char *)));
        if(c == 'x')
                return(ft_putnbr(va_arg(*args,unsigned),16,"0123456789abcdef"));
        if(c == 'X')
                return (ft_putnbr(va_arg(*args,unsigned),16 "0123456789ABCDEF"));
        if (c == '%')
                return(ft_putchar('%'));
        return 0;
}
int ft_printf(const char *str, ...)
{
        va_list args;
        int len = 0;
        int ret;
        va_start(args,str);
        while(*str)
        {
                if(*str == '%')
                        ret = ft_format(&args,*str++);
                else
                        ret = write(1,str,1);
                if(ret == -1)
                        return(va_end(args), -1);
                
        }
}