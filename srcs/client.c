/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsekiguc <tsekiguc@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/10 10:47:56 by tsekiguc          #+#    #+#             */
/*   Updated: 2021/11/09 11:28:31 by tsekiguc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"

static void	send_bit(unsigned char sent_char, pid_t pid)
{
	unsigned char	answer;
	unsigned char	bit;

	bit = 1 << 7;
	while (bit != 0)
	{
		answer = sent_char & bit;
		if (answer == 0)
		{
			if (kill(pid, SIGUSR1) < 0)
			{
				ft_putendl_fd("kill error in send bit", STDERR_FILENO);
				exit(1);
			}
		}
		else
		{
			if (kill(pid, SIGUSR2) < 0)
			{
				ft_putendl_fd("kill error in send bit", STDERR_FILENO);
				exit(1);
			}
		}
		if (usleep(100) < 0)
		{
			//int num;

			ft_putendl_fd("usleep error in send bit", STDERR_FILENO);
//			num = errno;
//			perror("error is ");
			exit(1);
		}
		bit >>= 1;
	}
}

static void	handle_signal(int signal)
{
	if (signal == SIGUSR1)
		ft_putendl_fd("ACK recieved ;)", STDOUT_FILENO);
	exit(1);
}

static void	error_exit(char *msg)
{
	ft_putendl_fd(msg, STDERR_FILENO);
	exit(1);
}

int	main(int argc, char *argv[])
{
	struct sigaction	sa;
	sigset_t			block;
	pid_t				pid;
	size_t				i;

	if (argc != 3)
	{
		ft_putendl_fd("\"./client [server PID] [message]\"", STDOUT_FILENO);
		return (0);
	}

	if ((sigemptyset(&block) != 0)
		|| (sigaddset(&block, SIGUSR1) != 0))
		error_exit("sigset error in main");

	ft_memset(&sa, '\0', sizeof(sa));
	sa.sa_handler = &handle_signal;
	sa.sa_mask = block;
	sa.sa_flags = SA_RESTART;

	if (sigaction(SIGUSR1, &sa, NULL) < 0)
	{
		ft_putendl_fd("sigaction error in main", STDERR_FILENO);
		return (1);
	}

	pid = (pid_t)ft_atoi(argv[1]);
	if (pid <= 0)
	{
		ft_putendl_fd("Please input server PID", STDOUT_FILENO);
		return (0);
	}

	i = 0;
	while (argv[2][i] != '\0')
	{
		send_bit((unsigned char)argv[2][i], pid);
		i++;
	}
	send_bit('\0', pid);
	pause();
	return (0);
}
