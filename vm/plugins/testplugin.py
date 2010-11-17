import cli

def callback():
    # print 'called'
    cli.specify_vm_interrupt(simulator, cli.INTERRUPT_TIMER, 1)

cli.register_callback(simulator, callback)
