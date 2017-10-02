/* { dg-do compile } */
/* { dg-additional-options "-fdump-rtl-expand-all" */
int isdigit(int c)
{
        return c >= 0;
}


