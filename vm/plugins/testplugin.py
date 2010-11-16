import cli

def callback():
    print 'called'
    cli.specify_vm_interrupt(simulator, cli.INTERRUPT_TIMER, 1)
    # raise NameError("I am an error in an interrupt callback yaaay!")

cli.register_callback(simulator, callback)