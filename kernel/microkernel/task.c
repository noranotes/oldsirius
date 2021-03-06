#include <io.h>
#include <tss.h>
#include <task.h>



// Aqui!A coisa começa a ser mais  interessante, a qui!
// Se pretende crias nossas estruturas de Multitasking
// OBS: Minhas estruturas de tarefa
// estão em fase de teste e criação


volatile task_t *current_task; // A tarefa actualmente em execunção
volatile task_t *ready_queue; // O início da lista.
int task_poiter[17];

// O próximo ID do processo disponível.
uint32_t  next_pid = 0;   

void init_tasking(){

    	cli();
   
    	current_task = ready_queue =(task_t*)malloc(sizeof(task_t));
    	current_task->id = next_pid++;
    	current_task->regs.eax = 0;
    	current_task->regs.ebx = 0;
    	current_task->regs.ecx = 0;
    	current_task->regs.edx = 0;
    	current_task->regs.edi = 0;
    	current_task->regs.esi = 0;
    	current_task->regs.ebp = 0;
    	current_task->regs.esp = 0;
    	current_task->regs.eip = 0;
    	current_task->regs.eflag =0x3202;
    	current_task->regs.cs = 0x8;
    	current_task->regs.ds = 0x10;
    	current_task->regs.es = 0x10;
    	current_task->regs.fs = 0x10;
    	current_task->regs.gs = 0x10;
    	current_task->regs.ss = 0x10;
    	current_task->regs.PDBR = (uint32_t)page->kpdbr;
    	current_task->esp0 = (uint32_t)kalloc(2);   
    	current_task->next = NULL;

    
    	sti();

}



// Todas os processos serão criados a partir desta função
int create_task(void (*main)(),page_directory_t *page_directory,uint32_t stack,uint32_t stack0,uint8_t privileg)
{

    task_t* new_task    =(task_t*)malloc(sizeof(task_t)); 
    new_task->id = next_pid++;
    new_task->regs.eax = 0;
    new_task->regs.ebx = 0;
    new_task->regs.ecx = 0;
    new_task->regs.edx = 0;
    new_task->regs.edi = 0;
    new_task->regs.esi = new_task->id;
    new_task->regs.ebp = 0;
    new_task->regs.esp = stack;
    if(privileg) { // processo do userspace
    new_task->regs.cs = 0x1B;
    new_task->regs.ds = 0x23;
    new_task->regs.es = 0x23;
    new_task->regs.fs = 0x23;
    new_task->regs.gs = 0x23;
    new_task->regs.ss = 0x23;

    new_task->esp0      = stack0;

    }else{ // processo do kernelspace
    new_task->regs.cs = 0x8;
    new_task->regs.ds = 0x10;
    new_task->regs.es = 0x10;
    new_task->regs.fs = 0x10;
    new_task->regs.gs = 0x10;
    new_task->regs.ss = 0x10;

    }

    new_task->regs.eflag =0x3202;
    new_task->regs.eip  = (uint32_t)main;   
    new_task->regs.PDBR = (uint32_t)page_directory;

    new_task->next = NULL;

    
     cli();
    // Adicionar novo elemento, no final da lista
    // tmp aponta para inicio da lista
    task_t* tmp = (task_t*) ready_queue;
    while (tmp->next)
    tmp = (task_t*)tmp->next;
    
    tmp->next = (task_t*) new_task;
    sti();


   return (new_task->id);
}



void task_switch(void){

    
    
    // Salve o contexto da actual tarefa
    // em execução   
    current_task->regs.eax  = task_poiter[0];
    current_task->regs.ebx  = task_poiter[1];
    current_task->regs.ecx  = task_poiter[2];
    current_task->regs.edx  = task_poiter[3];
    current_task->regs.edi  = task_poiter[4];
    current_task->regs.esi  = task_poiter[5];
    current_task->regs.ebp  = task_poiter[6];
    current_task->regs.esp  = task_poiter[7];
    
    current_task->regs.cs   = task_poiter[8];
    current_task->regs.ds   = task_poiter[9];
    current_task->regs.es   = task_poiter[10];
    current_task->regs.fs   = task_poiter[11];
    current_task->regs.gs   = task_poiter[12];
    current_task->regs.ss   = task_poiter[13];

    current_task->regs.eflag   = task_poiter[14];
    current_task->regs.eip      = task_poiter[15];
    current_task->regs.PDBR     = task_poiter[16];
    current_task->esp0          = task_poiter[17];

    // Pula, se ainda não tiver 
    // a tarefa inicializada...
    if (!current_task);
    else{

    // Obter a próxima tarefa a ser executada.
    current_task = current_task->next;

    // Se caímos no final da lista vinculada, 
    // comece novamente do início.
    if (!current_task)
    current_task = ready_queue;

    }
    // Restaura o contexto da próxima
    // tarefa a ser executada...
    task_poiter[0] = current_task->regs.eax;
    task_poiter[1] = current_task->regs.ebx;
    task_poiter[2] = current_task->regs.ecx;
    task_poiter[3] = current_task->regs.edx;
    task_poiter[4] = current_task->regs.edi;
    task_poiter[5] = current_task->regs.esi;
    task_poiter[6] = current_task->regs.ebp;
    task_poiter[7] = current_task->regs.esp;
    
    task_poiter[8] = current_task->regs.cs;
    task_poiter[9] = current_task->regs.ds;
    task_poiter[10] = current_task->regs.es;
    task_poiter[11] = current_task->regs.fs;
    task_poiter[12] = current_task->regs.gs;
    task_poiter[13] = current_task->regs.ss;

    task_poiter[14] = current_task->regs.eflag;
    task_poiter[15] = current_task->regs.eip;
    task_poiter[16] = current_task->regs.PDBR;
    task_poiter[17] = current_task->esp0;

    //FIXME Aqui podemos mudar, cr3   
    if(current_task->regs.PDBR)
    load_page_diretory((page_directory_t*)current_task->regs.PDBR);

    // stack switch
    if(current_task->esp0)
    stack0(current_task->esp0);

    
}

// Retorna o pid do processo atual.
int getpid()
{
    return current_task->id;
}
int getcr3()
{
    return current_task->regs.PDBR;
}
