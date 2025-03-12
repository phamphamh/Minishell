/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:19:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/12 13:43:43 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

/**
 * @brief Gestionnaire pour le signal SIGINT (Ctrl+C)
 *
 * @param sig Numéro du signal reçu
 */
static void	ft_handle_sigint(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	g_signal_received = 130;
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/**
 * @brief Gestionnaire pour le signal SIGINT dans un heredoc
 *
 * @param sig Numéro du signal reçu
 */
static void	ft_handle_heredoc_sigint(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	g_signal_received = 130;
	exit(130);
}

/**
 * @brief Configure les gestionnaires de signaux pour le shell interactif
 */
void	ft_setup_signals(void)
{
	struct sigaction	sa_int;

	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_handler = ft_handle_sigint;
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	signal(SIGQUIT, SIG_IGN);
}

/**
 * @brief Réinitialise les gestionnaires de signaux aux comportements par défaut
 */
void	ft_reset_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

/**
 * @brief Configure les signaux en mode ignoré
 */
void	ft_ignore_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

/**
 * @brief Gestionnaire de signal spécifique pour le heredoc
 */
void	ft_heredoc_signals(void)
{
	struct sigaction	sa_int;

	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_handler = ft_handle_heredoc_sigint;
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	signal(SIGQUIT, SIG_IGN);
}
