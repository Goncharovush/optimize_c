#from MySpice import MySpice as spice


def simulate(name):
    circuit = spice.LoadFile(str(name))
    input_data = spice.Init_Data(1000, 0.3, SNR=10 ** 6)
    analysis = spice.CreateCVC(circuit, input_data, 100)
    ivc = [analysis.input_dummy, analysis.VCurrent]
    return ivc
    
def example(name):
    print("This is example")
    return [(0, 0, 0,), (1, 2, 3)]