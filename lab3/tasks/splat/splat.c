/** @file splat.c
 *
 * @brief Displays a spinning cursor.
 * @author Marc-Daniel Julien <mjulien@andrew.cmu.edu>
 * @author Jeffery He <jmhe@andrew.cmu.edu>
 * 
 * Links to libc.
 */

 #include <unistd.h>

int main(int argc, char** argv)
{
	char backspace[] = {'\b', ' ', '\b'};
	char glyphs[] = {'-', '\\', '|', '/', '-', '\\', '|', '/'};
	int i = 0;
	while(1)
	{
		/* Write glyph */
		write(STDOUT_FILENO, &(glyphs[(i++)%8]), 1);
		
		/* Wait */
		sleep(200);

		/* Write backspace and erase glyth */
		write(STDOUT_FILENO, backspace, 3);
	}
	return 0;
}
