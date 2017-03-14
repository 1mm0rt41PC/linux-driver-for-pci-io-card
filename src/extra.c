#include "extra.h"
#include <linux/module.h>
#include <linux/fs.h>// Pour créer un accès à /dev/...
#include <linux/string.h>

static char G_fullpath[1024] = {0};


const char* getFullPath( const struct file* pfile )
{
	//tmp = (char *)__get_free_page(GFP_TEMPORARY);
	//pathname = d_path(path, G_fullpath, PAGE_SIZE);
	//stdError(KERN_DEBUG, "filepath=%s", pathname);
	// USE IT HERE
	//free_page((unsigned long)tmp);
	char *ret = 0;
	ret = d_path(&pfile->f_path, G_fullpath, 1024-1);
	return ret;
}


/****************************************************************************//*!
* @brief Permetde trouver l'ID de la card à contacter
* @param[in]  pfile		Le Context
* @param[out] card_w	ID pour la sortie
* @param[out] card_r	ID pour l'entrée
* @return
*	-1: Erreur
*	 1: ok
*
* Format autorisé:
*	[0-9]+
*	card_w[0-9]+r[0-9]+
*/
char getID( const struct file* pfile, int* card_w, int* card_r )
{
	int i=0;
	char* ret = 0;

	ret = d_path(&pfile->f_path, G_fullpath, 1024-1);
	
	for( i=0; ret[i]; ++i );
	for( i=i-1; ret[i]!='/'; --i );
	
	if( strncmp(ret+i+1,"card_w", 6) == 0 ){
		*card_w = str2int(ret+i+1+6);
		for( i=i+1+6; ret[i]!='_'; ++i );
		*card_r = str2int(ret+i+1+1);
	}else{
		*card_w = str2int(ret+i+1);
		*card_r = str2int(ret+i+1);
	}
	
	if( *card_w <= 0 || *card_r <= 0 ){
		stdError(KERN_DEBUG, "invalid driver name: <%s>", ret);
		return 0;
	}
	return 1;
}



int str2int( const char str[] )
{
	int i=0;
	int ret=0;
	for( i=0; str[i]; ++i )
	{
		if( !('0' <= str[i] && str[i] <= '9') ){
			if( i == 0 )
				return -1;
			return ret;
		}
		ret *= 10;
		ret += str[i]-'0';
	}
	return ret;
}
