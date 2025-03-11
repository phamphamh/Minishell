/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:19:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 13:45:38 by yboumanz         ###   ########.fr       */
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

	// Ajouter un saut de ligne et un nouveau prompt
	write(STDOUT_FILENO, "\n", 1);

	// Marquer le signal comme reçu
	g_signal_received = 130; // Code de sortie pour Ctrl+C

	// Réinitialiser l'état de readline pour éviter les problèmes
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/**
 * @brief Configure les gestionnaires de signaux pour le shell interactif
 */
void	ft_setup_signals(void)
{
	// Pour SIGINT (Ctrl+C), utiliser sigaction pour plus de contrôle
	struct sigaction	sa_int;

	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_handler = ft_handle_sigint;
	sa_int.sa_flags = 0; // Pas de SA_RESTART
	sigaction(SIGINT, &sa_int, NULL);

	// Pour SIGQUIT (Ctrl+\), simplement ignorer
	signal(SIGQUIT, SIG_IGN);
}

/**
 * @brief Réinitialise les gestionnaires de signaux aux comportements par défaut
 */
void	ft_reset_signals(void)
{
	// Rétablir le comportement par défaut pour les processus enfants
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

/**
 * @brief Configure les signaux en mode ignoré
 */
void	ft_ignore_signals(void)
{
	// Ignorer les signaux pendant l'attente des processus enfants
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

/**
 * @brief Gestionnaire de signal spécifique pour le heredoc
 */
void	ft_heredoc_signals(void)
{
	// Configuration spécifique pour les heredocs
	signal(SIGINT, SIG_DFL);  // Laisser le heredoc être interrompu
	signal(SIGQUIT, SIG_IGN); // Ignorer Ctrl+\ pendant le heredoc
}
