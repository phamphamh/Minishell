/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:19:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/01 23:04:21 by tcousin          ###   ########.fr       */
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
	g_signal_received = 1;            // Indique qu'un signal SIGINT a été reçu
	write(1, "\n", 1);                // Nouvelle ligne
	rl_on_new_line();                // Indique à readline qu'on est sur une nouvelle ligne
	rl_redisplay();                  // Réaffiche le prompt
}

/**
 * @brief Gestionnaire pour le signal SIGQUIT (Ctrl+\)
 *
 * @param sig Numéro du signal reçu
 */
static void	ft_handle_sigquit(int sig)
{
	(void)sig;
	// Dans le shell interactif, SIGQUIT (Ctrl+\) est ignoré
	// Le gestionnaire est vide, mais on le définit explicitement pour clarté
}

/**
 * @brief Configure les gestionnaires de signaux pour le shell interactif
 */
void	ft_setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	// Réinitialiser l'état du signal
	g_signal_received = 0;

	// Configurer SIGINT (Ctrl+C)
	sa_int.sa_handler = ft_handle_sigint;
	sa_int.sa_flags = SA_RESTART;  // Ajouter SA_RESTART pour éviter d'interrompre certaines fonctions système
	sigemptyset(&sa_int.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);

	// Configurer SIGQUIT (Ctrl+\)
	sa_quit.sa_handler = ft_handle_sigquit;
	sa_quit.sa_flags = SA_RESTART;  // Ajouter SA_RESTART pour cohérence
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

/**
 * @brief Réinitialise les gestionnaires de signaux aux comportements par défaut
 */
void	ft_reset_signals(void)
{
	// Dans les processus enfants, rétablir le comportement par défaut
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	g_signal_received = 0;  // Réinitialiser l'état du signal
}

/**
 * @brief Configure les signaux en mode ignoré
 */
void	ft_ignore_signals(void)
{
	// Pendant l'attente des processus enfants
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}
