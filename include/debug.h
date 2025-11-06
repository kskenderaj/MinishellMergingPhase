/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 00:00:00 by kskender          #+#    #+#             */
/*   Updated: 2025/11/03 14:17:03 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_H
# define DEBUG_H

# include "libft.h"
# include "parser.h"
# include <stdarg.h>

# define COLOR_RED "\x1b[31m"
# define COLOR_GREEN "\x1b[32m"
# define COLOR_YELLOW "\x1b[33m"
# define COLOR_BLUE "\x1b[34m"
# define COLOR_CYAN "\x1b[36m"
# define COLOR_RESET "\x1b[0m"

void	print_tokens(const t_token_list *list);
void	debug_printf(const char *color, const char *label, const char *fmt,
			...);

/* Debug macros */
# if defined(CURRENT_DEBUG_LEVEL) && (CURRENT_DEBUG_LEVEL >= DBG_LEVEL_ERROR)
#  define DEBUG_ERROR(fmt, ...) debug_printf(COLOR_RED, "[ERROR] ", fmt,
	##__VA_ARGS__)
# else
#  define DEBUG_ERROR(fmt, ...)
# endif

# if defined(CURRENT_DEBUG_LEVEL) && (CURRENT_DEBUG_LEVEL >= DBG_LEVEL_WARN)
#  define DEBUG_WARN(fmt, ...) debug_printf(COLOR_YELLOW, "[WARN] ", fmt,
	##__VA_ARGS__)
# else
#  define DEBUG_WARN(fmt, ...)
# endif

# if defined(CURRENT_DEBUG_LEVEL) && (CURRENT_DEBUG_LEVEL >= DBG_LEVEL_INFO)
#  define DEBUG_INFO(fmt, ...) debug_printf(COLOR_BLUE, "[INFO] ", fmt,
	##__VA_ARGS__)
# else
#  define DEBUG_INFO(fmt, ...)
# endif

# if defined(CURRENT_DEBUG_LEVEL) && (CURRENT_DEBUG_LEVEL >= DBG_LEVEL_TRACE)
#  define DEBUG_TRACE(fmt, ...) debug_printf(COLOR_CYAN, "[TRACE] ", fmt,
	##__VA_ARGS__)
# else
#  define DEBUG_TRACE(fmt, ...)
# endif

#endif
