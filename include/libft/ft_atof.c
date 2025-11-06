/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 15:05:58 by kskender          #+#    #+#             */
/*   Updated: 2025/07/24 15:08:27 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static double	handle_decimal(const char **str)
{
	double	decimal;
	double	factor;

	decimal = 0.0;
	factor = 0.1;
	if (**str != '.')
		return (0.0);
	(*str)++;
	while (ft_isdigit(**str))
	{
		decimal += (**str - '0') * factor;
		factor *= 0.1;
		(*str)++;
	}
	return (decimal);
}

double	ft_atof(const char *str)
{
	double	result;
	int		sign;

	result = 0.0;
	sign = 1;
	while (*str == 32 || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (ft_isdigit(*str))
	{
		result = result * 10 + (*str - '0');
		str++;
	}
	result += handle_decimal(&str);
	return (result * sign);
}
