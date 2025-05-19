using System;
using System.Numerics;

public class Qubit
{
    public Complex Alpha { get; set; }
    public Complex Beta { get; set; }

    public Qubit(Complex alpha, Complex beta)
    {
        if (Math.Abs((alpha.Magnitude * alpha.Magnitude) + (beta.Magnitude * beta.Magnitude) - 1.0) > 1e-10)
            throw new ArgumentException("Quantum state must be normalized");

        Alpha = alpha;
        Beta = beta;
    }

    public Qubit() : this(new Complex(1, 0), new Complex(0, 0)) { }

    public override string ToString() => $"{Alpha}|0⟩ + {Beta}|1⟩";
}

public static class QuantumGates
{
    public static void PauliX(Qubit q) => (q.Alpha, q.Beta) = (q.Beta, q.Alpha);

    public static void PauliY(Qubit q)
    {
        var newAlpha = Complex.ImaginaryOne * q.Beta;
        var newBeta = -Complex.ImaginaryOne * q.Alpha;
        q.Alpha = newAlpha;
        q.Beta = newBeta;
    }

    public static void PauliZ(Qubit q) => q.Beta = -q.Beta;

    public static void Hadamard(Qubit q)
    {
        var sqrt2 = Math.Sqrt(2);
        var newAlpha = (q.Alpha + q.Beta) / sqrt2;
        var newBeta = (q.Alpha - q.Beta) / sqrt2;
        q.Alpha = newAlpha;
        q.Beta = newBeta;
    }

    public static void CNot(Qubit control, Qubit target)
    {
        double prob1 = (control.Beta.Magnitude * control.Beta.Magnitude);
        if (new Random().NextDouble() < prob1)
            PauliX(target);
    }
}

class Program
{
    static void Main()
    {
        Console.WriteLine("Quantum Gates Demonstration:");

        var q1 = new Qubit();
        Console.WriteLine($"Initial state: {q1}");

        QuantumGates.Hadamard(q1);
        Console.WriteLine($"After Hadamard: {q1}");

        QuantumGates.PauliX(q1);
        Console.WriteLine($"After Pauli X: {q1}");

        QuantumGates.PauliY(q1);
        Console.WriteLine($"After Pauli Y: {q1}");

        QuantumGates.PauliZ(q1);
        Console.WriteLine($"After Pauli Z: {q1}");

        var controlQubit = new Qubit(new Complex(1/Math.Sqrt(2), 0), new Complex(1/Math.Sqrt(2), 0));
        var targetQubit = new Qubit();

        Console.WriteLine($"\nCNOT Demonstration:");
        Console.WriteLine($"Control qubit: {controlQubit}");
        Console.WriteLine($"Target qubit: {targetQubit}");

        QuantumGates.CNot(controlQubit, targetQubit);
        Console.WriteLine($"After CNOT:");
        Console.WriteLine($"Control qubit: {controlQubit}");
        Console.WriteLine($"Target qubit: {targetQubit}");
    }
}
