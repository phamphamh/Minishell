/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 12:47:33 by jspitz            #+#    #+#             */
/*   Updated: 2024/12/28 16:24:46 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

/**
* @brief use to clear the buffer while we navigate through it
*		 to concatenate both chain avoid useless leak, we add
*        buff1 to buff2 wich is empty at firtst because we wont need
*		 the original buffer after.
*
* @param buff1 the buff who is free
* @param buff2 the one who got buff memory
*
* @return newly created array  
*/
static char	*ft_fr(char *buff1, char *buff2)
{
	char	*temp;

	temp = ft_strjoin(buff1, buff2);
	free(buff1);
	return (temp);
}

/**
* @brief this function fill the newly created char chain with the line being 
*		 read, it
*		 fill the array of char created by read the file.
*
* @param buffer the buffer being read to write the line
*
* @return 
*/
static char	*ft_write_line(char *buffer)
{
	int		i;
	int		j;
	char	*new;

	i = 0;
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (!buffer[i])
	{
		free(buffer);
		return (NULL);
	}
	new = ft_calloc((ft_strlen(buffer) - i + 1), sizeof(*buffer));
	if (!new)
		return (NULL);
	j = 0;
	i++;
	while (buffer[i])
		new[j++] = buffer[i++];
	buffer[j] = '\0';
	free(buffer);
	return (new);
}

/**
* @brief this function create the buffer with the file we get as parameter,
*		 if the buff does not exist, we create a small one to avoid leak
*		 then we use the variable bytes to count how many has been read
*		 if bytes goes to -1, means the file is now over and there is nothing
*		 to read anymore. so we free new and the buffer,
*		 ft_fr is here to clean, the break '\n' condition to stop at each
*		 newly read line.
*
* @param fd the file we're reading
* @param buff the buffer that will receive the file.
*
* @return buffer without the line read,
*/
static char	*ft_read_the_file(int fd, char *buff)
{
	char	*new;
	int		bytes;

	if (!buff)
		buff = ft_calloc(1, 1);
	new = ft_calloc(BUFFER_SIZE + (int) 1, sizeof(char));
	if (!new)
		return (NULL);
	bytes = 1;
	while (bytes > 0)
	{
		bytes = read(fd, new, BUFFER_SIZE);
		if (bytes == -1)
		{
			free(new);
			free(buff);
			return (NULL);
		}
		new[bytes] = 0;
		buff = ft_fr(buff, new);
		if (ft_strchr(new, '\n'))
			break ;
	}
	free (new);
	return (buff);
}

/**
* @brief we create the char chain to save a line, 
*		 we go throug till end or '\n' is found
*
* @param buffer the buffer created before with the read file
*
* @return a line newly created
*/
static char	*ft_get_new_line(char *buffer)
{
	char	*new;
	int		i;

	i = 0;
	if (!buffer[i])
		return (NULL);
	while (buffer[i] && buffer[i] != '\n')
		i++;
	new = ft_calloc(i + 2, sizeof(char));
	if (!new)
		return (NULL);
	i = 0;
	while (buffer[i] && buffer[i] != '\n')
	{
		new[i] = buffer[i];
		i++;
	}
	if (buffer[i] && buffer[i] == '\n')
		new[i++] = '\n';
	return (new);
}

/**
* @brief read check if buffer size is ok and if fd exist
*		 then call the function to read the file and copy
*        line by line. the static is here because we want
*		 the buffer not to erase during the program,
*		 the different function will concatenate the byte
*		 read in the file.
*
* @param fd the descriptor of the file we read
*
* @return the newly read line
*/
char	*get_next_line(int fd)
{
	static char	*buffer[1024];
	char		*new_line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	buffer[fd] = ft_read_the_file(fd, buffer[fd]);
	if (!buffer[fd])
		return (NULL);
	new_line = ft_get_new_line(buffer[fd]);
	buffer[fd] = ft_write_line(buffer[fd]);
	return (new_line);
}
