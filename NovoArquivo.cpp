void detectaColisao(void){
     int x,y,z;
     invasores *inv;
     tiros *tn = listaTirosNave;
     while(tn != NULL){  //  colisão entre tiro nave e invasores
           if(tn->translacao.y < 2000){  
                 inv = listaInvasor;
                 while(inv != NULL){
                        if(inv->situacao == VIVO){  
                              x = tn->translacao.x  -  inv->translacao.x;
                              y = tn->translacao.y  -  inv->translacao.y;
                              z = tn->translacao.z  -  inv->translacao.z;
                              if((raioColisaoInvasor + tn->raioColisaoTiro) >= ( sqrt(x*x + y*y + z*z) )){
                                  inv->situacao = FOGO1;
                                  tn->translacao.y = 2500;
                                  invasoresVivos--;
                                  pontuacao = pontuacao + 15;
                                  //engine->play2D("som/cannon_x.wav");
                              }
                        }
                        inv = inv->prox;
                 }
           }
           tn = tn->prox;
     }
     //  colisão entre tiros dos invasore com a nave amiga
     tn = listaTirosInvasor;
     while(tn != NULL){ 
           if(tn->translacao.y > 0){  
                              x = tn->translacao.x  -  nave.translacao.x;
                              y = tn->translacao.y  -  nave.translacao.y;
                              z = tn->translacao.z  -  nave.translacao.z;
                              if((raioColisaoNave + tn->raioColisaoTiro) >= ( sqrt(x*x + y*y + z*z) )){
                                  vida = vida - 1;
                                  tremerCamera = true;
                                  contTremeCamera = 0;
                                  if(vida <= 0){
                                       // game over   chime_big_ben
                                      // engine->play2D("som/buzzer_x.wav");
                                       nave.situacao = FOGO1;
                                       vida = 0;
                                  }else{
                                       if(vida > 0) nave.situacao = FOGO1;
                                       tn->translacao.y = 0;
                                  }
                                 //engine->play2D("som/cannon_x.wav");
                              }
           }
           tn = tn->prox;
     } 
     //  colisão entre tiros dos invasore com tiro da nave amiga
     tiros *tiroInvasor = listaTirosInvasor;
     tiros *tirosNave = listaTirosNave;
     while(tirosNave != NULL){ 
           if(tirosNave->translacao.y < 2000){  
                 while(tiroInvasor != NULL){
        //                if(inv->situacao == VIVO){  
                              x = tirosNave->translacao.x  -  tiroInvasor->translacao.x;
                              y = tirosNave->translacao.y  -  tiroInvasor->translacao.y;
                              z = tirosNave->translacao.z  -  tiroInvasor->translacao.z;
                              if((tirosNave->raioColisaoTiro + tiroInvasor->raioColisaoTiro) >= ( sqrt(x*x + y*y + z*z) )){
        //                          inv->situacao = FOGO1;
                                  tirosNave->translacao.y = 2500;
                                  tiroInvasor->translacao.y = 0;
        //                          invasoresVivos--;
        //                          pontuacao = pontuacao + 15;
        //                          engine->play2D("som/cannon_x.wav");
                              }
        //                }
                        tiroInvasor = tiroInvasor->prox;
                 }
           }
           tirosNave = tirosNave->prox;
     }
     //  verifica se algun invasor toco no chao
     inv = listaInvasor;
     while(inv != NULL){
          if(inv->translacao.y <= 50  &&  vida > 0 && inv->situacao == VIVO){
              vida = 0;
              tremerCamera = true;
              contTremeCamera = 0;
              nave.situacao = FOGO1; 
             // engine->play2D("som/buzzer_x.wav");
 }  
          inv = inv->prox;
     }  
     return;
}
 void addTirosJogo(tiros *t){ 
     if(t !=NULL){
         if(t->tipo == TIRONAVE){
                 if(listaTirosNave != NULL){
                      t->prox = listaTirosNave;
                      listaTirosNave->anter = t;
                      t->anter = t;
                      listaTirosNave = t;
                 }else{
                      t->prox = NULL;
                      t->anter = t;
                      listaTirosNave = t;
                 }
         }else{// tipo == tiro invasor
                 if(listaTirosInvasor != NULL){
                      t->prox = listaTirosInvasor;
                      listaTirosInvasor->anter = t;
                      t->anter = t;
                      listaTirosInvasor = t;
                 }else{
                      t->prox = NULL;
                      t->anter = t;
                      listaTirosInvasor = t;
                 }
         }
     }

     
     
/*
     if(t->tipo == TIRONAVE){
           if(listaTirosNave != NULL){
                t->prox = listaTirosNave;
           }
           t->anter = t;
           listaTirosNave = t;
     }else{
           if(listaTirosInvasor != NULL){
                t->prox = listaTirosInvasor;
           }
           t->anter = t;
           listaTirosInvasor = t;
     }
*/
     return;
}

/*                                                                    */
/*                    Função para desenpilhar tiros                   */
/*                                                                    */
/**********************************************************************/
void remTirosJogo(tiros *t){   
     if(t != NULL){
         if(t->tipo == TIRONAVE){
              if(t->anter != t){
                   t->anter->prox = t->prox;
              }else{
                   listaTirosNave == t->prox;
                   listaTirosNave->anter = listaTirosNave;
              }
              free(t);
         }else{
              if(t->anter != t){
                   t->anter->prox = t->prox;
              }else{
                   listaTirosInvasor == t->prox;
                   listaTirosInvasor->anter = listaTirosInvasor;
              }
              free(t);
         }
     }
     return;
}
