/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 02:03:10 by merilhan          #+#    #+#             */
/*   Updated: 2025/06/17 03:03:00 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>

const   char *builtin_command[] = 
{
        "echo",
        "cd",
        "pwd",
        "exit",
        NULL
};

int     is_builtin(char *av)
{
        int i = 0;
        while(builtin_command[i])
        {
                if(strcmp(av,builtin_command[i]) == 0)
                        return i;
        }
        return -1;
}
int     built_echo(char **av)
{
        int i = 1; // çünkü ilk'i echo 
        while(av[i])
        {
                printf("%s",av[i]);
                        if(av[i + 1] != NULL)
                                printf(" ");
                        i++;
        }
        printf("\n");
        return 0;
}
int     built_cd(char **av)
{
        if(av[1] == NULL)
        {
                fprintf(stderr,"cd : argument missing\n");
                return 1;
        }
        if(chdir(av[1]) != 0)
        {
                perror("cd");
                return 1;
        }
        return 0;
}
int     builtin_pwd(char **av)
{
        char cwd[PATH_MAX]; // env ile alakali // never forget this env
        if(getcwd(cwd,sizeof(cwd)) != NULL)
        {
                printf("%s\n",cwd);
                return 0;
        }
        else
        {
                perror("pwd");
                return 1;
        }
}
int builtin_exit(char **av)
{
        int code = 0; // çıkış kodu dedik default 
        if(av[1] != NULL)
                code = atoi(av[1]); // input olarak alıyoruz verirsek 
        printf("exit\n");
        exit(code);
        return 0;
}
int run_builtin(char **av)
{
        int id;
        id = is_builtin(av[0]);
        if(id == -1)
                return -1;
        if(id == 0)
                return (builtin_echo(av));
        else if(id == 1)
                return (built_cd(av));
        else if(id == 2)
                return (builtin_pwd(av));
        else if (id == 3)
                return (builtin_exit(av));
        else
                return -1
}