# include <stdarg.h>
# include <stdint.h>
# include <stddef.h>
# include "../kernel/terminal.h"
# include "lib.h"

static int	ft_unknow_percent(const char *str, int *l)
{
	int	temp;

	temp = terminal_write(str, 2);
	*l += temp;
	if (temp == -1)
		return (1);
	return (0);
}


static int	ft_putchar(char c, int *l)
{
	int	temp;

	temp = terminal_write(&c, 1);
	*l += temp;
	if (temp == -1)
		return (1);
	return (0);
}

static int	ft_putstr(char *s, int *l)
{
	int	temp;

	if (!s)
	{
		temp = terminal_write("(null)", 6);
		*l += temp;
		if (temp == -1)
			return (1);
	}
	else
	{
		temp = terminal_write(s, strlen(s));
		*l += temp;
		if (temp == -1)
			return (1);
	}
	return (0);
}

static int	ft_putnbr(long nbr, int base, int uppercase, int *l)
{
	char	*table;

	table = "0123456789abcdef0123456789ABCDEF";
	if (nbr == -2147483648)
	{
		if (ft_putstr("-2147483648", l))
			return (1);
		return (0);
	}
	if (nbr < 0)
	{
		if (ft_putchar('-', l))
			return (1);
		nbr *= -1;
	}
	if (nbr >= base)
	{
		if (ft_putnbr(nbr / base, base, uppercase, l))
			return (1);
	}
	if (ft_putchar(table[nbr % base + uppercase], l))
		return (1);
	return (0);
}

static int	ft_putptr_rec(size_t p, int *l, char *base)
{
	if (p >= 16)
		if (ft_putptr_rec(p / 16, l, base))
			return (1);
	if (ft_putchar(base[p % 16], l))
		return (1);
	return (0);
}

static int	ft_putptr(void *p, int *l)
{
	char	*base;

	base = "0123456789abcdef";
	if (p == NULL)
	{
		if (ft_putstr("(nil)", l))
			return (1);
		return (0);
	}
	else
	{
		if (ft_putstr("0x", l))
			return (1);
		if (ft_putptr_rec((size_t)p, l, base))
			return (1);
	}
	return (0);
}


static int	ft_percent(const char *str, int *l, va_list argptr)
{
	if (str[1] == 'c')
		return (ft_putchar(va_arg(argptr, int), l));
	else if (str[1] == 's')
		return (ft_putstr(va_arg(argptr, char *), l));
	else if (str[1] == 'p')
		return (ft_putptr(va_arg(argptr, void *), l));
	else if (str[1] == 'd' || str[1] == 'i')
		return (ft_putnbr(va_arg(argptr, int), 10, 0, l));
	else if (str[1] == 'u')
		return (ft_putnbr(va_arg(argptr, unsigned int), 10, 0, l));
	else if (str[1] == 'x')
		return (ft_putnbr(va_arg(argptr, unsigned int), 16, 0, l));
	else if (str[1] == 'X')
		return (ft_putnbr(va_arg(argptr, unsigned int), 16, 16, l));
	else if (str[1] == '%')
		return (ft_putchar('%', l));
	else
		return (ft_unknow_percent(str, l));
}

int	printk(const char *s, ...)
{
	int		l;
	size_t	i;
	va_list	argptr;

	i = -1;
	l = 0;
	if (!s)
		return (-1);
	va_start(argptr, s);
	while (++i < strlen(s))
	{
		if (s[i] == '%')
		{
			if (!s[i + 1])
				return (va_end(argptr), -1);
			if (ft_percent(s + i, &l, argptr))
				return (-1);
			i++;
		}
		else
			ft_putchar(s[i], &l);
	}
	va_end(argptr);
	return (l);
}