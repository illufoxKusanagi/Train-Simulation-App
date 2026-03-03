#!/usr/bin/env python3
"""
Generate Fuzzy Membership Function Graphs for Train Optimization Thesis
Creates publication-quality images of all membership functions

GOOGLE COLAB USAGE:
------------------
1. Copy this entire script into a Colab cell
2. Run the cell to define all functions
3. In a new cell, run:
   main()  # Display inline + save files
   
   Or customize:
   main(display_inline=True, save_files=True)   # Show + save
   main(display_inline=True, save_files=False)  # Show only
   
4. Download files from left sidebar: Files → fuzzy_graphs → Download

LOCAL USAGE:
-----------
python3 generate_fuzzy_membership_graphs.py
"""

import matplotlib.pyplot as plt
import numpy as np
import os


def trapezoid_membership(x, a, b, c, d):
    """
    Calculate trapezoid membership function value
    
    Parameters:
    - a, b: left foot and shoulder
    - c, d: right shoulder and foot
    
    Returns membership value between 0 and 1
    """
    if x <= a or x >= d:
        return 0.0
    elif a < x < b:
        return (x - a) / (b - a)
    elif b <= x <= c:
        return 1.0
    elif c < x < d:
        return (d - x) / (d - c)
    return 0.0


def plot_travel_time_membership():
    """Plot Travel Time membership functions (seconds)"""
    x = np.linspace(250, 950, 1000)
    
    # Define membership functions
    # Short: 300-450s, Medium: 400-650s, Long: 600-900s
    short = [trapezoid_membership(val, 300, 300, 400, 450) for val in x]
    medium = [trapezoid_membership(val, 400, 450, 600, 650) for val in x]
    long = [trapezoid_membership(val, 600, 650, 900, 900) for val in x]
    
    plt.figure(figsize=(10, 6))
    plt.fill_between(x, 0, short, alpha=0.3, color='darkgreen', label='Short (Good)')
    plt.plot(x, short, color='darkgreen', linewidth=2.5)
    plt.fill_between(x, 0, medium, alpha=0.3, color='orange', label='Medium')
    plt.plot(x, medium, color='orange', linewidth=2.5)
    plt.fill_between(x, 0, long, alpha=0.3, color='red', label='Long (Bad)')
    plt.plot(x, long, 'r-', linewidth=2.5)
    
    plt.xlabel('Travel Time (seconds)', fontsize=12, fontweight='bold')
    plt.ylabel('Membership Degree', fontsize=12, fontweight='bold')
    plt.title('Travel Time Membership Functions', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11, loc='upper right')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.xlim(250, 950)
    plt.ylim(-0.05, 1.1)
    plt.tight_layout()
    
    return plt.gcf()


def plot_motor_power_membership():
    """Plot Motor Power per Car membership functions (kW)"""
    x = np.linspace(200, 1100, 1000)
    
    # Define membership functions based on common KRL specs
    # Low: 300-550kW (Good), Medium: 500-750kW, High: 700-1000kW (Bad)
    low = [trapezoid_membership(val, 300, 300, 500, 550) for val in x]
    medium = [trapezoid_membership(val, 500, 550, 700, 750) for val in x]
    high = [trapezoid_membership(val, 700, 750, 1000, 1000) for val in x]
    
    plt.figure(figsize=(10, 6))
    plt.fill_between(x, 0, low, alpha=0.3, color='darkgreen', label='Low (Good)')
    plt.plot(x, low, color='darkgreen', linewidth=2.5)
    plt.fill_between(x, 0, medium, alpha=0.3, color='orange', label='Medium')
    plt.plot(x, medium, color='orange', linewidth=2.5)
    plt.fill_between(x, 0, high, alpha=0.3, color='red', label='High (Bad)')
    plt.plot(x, high, 'r-', linewidth=2.5)
    
    plt.xlabel('Motor Power per Car (kW)', fontsize=12, fontweight='bold')
    plt.ylabel('Membership Degree', fontsize=12, fontweight='bold')
    plt.title('Motor Power per Car Membership Functions', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11, loc='upper right')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.xlim(200, 1100)
    plt.ylim(-0.05, 1.1)
    plt.tight_layout()
    
    return plt.gcf()


def plot_acceleration_membership():
    """Plot Acceleration membership functions (m/s²) - Design Parameter"""
    x = np.linspace(0.5, 1.3, 1000)
    
    # Define membership functions based on user requirements
    # Low: < 0.7, Medium: 0.7-0.9, High: > 1.0
    low = [trapezoid_membership(val, 0.5, 0.5, 0.65, 0.75) for val in x]
    medium = [trapezoid_membership(val, 0.7, 0.75, 0.85, 0.95) for val in x]
    high = [trapezoid_membership(val, 0.9, 1.0, 1.2, 1.2) for val in x]
    
    plt.figure(figsize=(10, 6))
    plt.fill_between(x, 0, low, alpha=0.3, color='blue', label='Low (< 0.7)')
    plt.plot(x, low, 'b-', linewidth=2.5)
    plt.fill_between(x, 0, medium, alpha=0.3, color='green', label='Medium (0.7-0.9)')
    plt.plot(x, medium, 'g-', linewidth=2.5)
    plt.fill_between(x, 0, high, alpha=0.3, color='red', label='High (> 1.0)')
    plt.plot(x, high, 'r-', linewidth=2.5)
    
    plt.xlabel('Acceleration (m/s²)', fontsize=12, fontweight='bold')
    plt.ylabel('Membership Degree', fontsize=12, fontweight='bold')
    plt.title('Acceleration Membership Functions (Design Parameter)', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11, loc='upper right')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.xlim(0.5, 1.3)
    plt.ylim(-0.05, 1.1)
    plt.tight_layout()
    
    return plt.gcf()


def plot_gradient_membership():
    """Plot Gradient membership functions (per-mill ‰)"""
    x = np.linspace(-5, 30, 1000)
    
    # Define membership functions in per-mill
    # Low: 0-5‰, Medium: ~10‰, High: 15-25‰
    low = [trapezoid_membership(val, 0, 0, 5, 8) for val in x]
    medium = [trapezoid_membership(val, 5, 8, 12, 15) for val in x]
    high = [trapezoid_membership(val, 12, 15, 25, 25) for val in x]
    
    plt.figure(figsize=(10, 6))
    plt.fill_between(x, 0, low, alpha=0.3, color='blue', label='Low (0-5‰)')
    plt.plot(x, low, 'b-', linewidth=2.5)
    plt.fill_between(x, 0, medium, alpha=0.3, color='green', label='Medium (~10‰)')
    plt.plot(x, medium, 'g-', linewidth=2.5)
    plt.fill_between(x, 0, high, alpha=0.3, color='red', label='High (15-25‰)')
    plt.plot(x, high, 'r-', linewidth=2.5)
    
    plt.xlabel('Gradient (‰ per-mill)', fontsize=12, fontweight='bold')
    plt.ylabel('Membership Degree', fontsize=12, fontweight='bold')
    plt.title('Gradient Membership Functions', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11, loc='upper right')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.xlim(-5, 30)
    plt.ylim(-0.05, 1.1)
    plt.axvline(x=0, color='k', linestyle=':', alpha=0.5, linewidth=1)
    plt.tight_layout()
    
    return plt.gcf()


def plot_fuzzy_score_output():
    """Plot Fuzzy Score output membership functions"""
    x = np.linspace(0, 1, 1000)
    
    # Define membership functions for quality/fitness score
    poor = [trapezoid_membership(val, 0, 0, 0.25, 0.35) for val in x]
    fair = [trapezoid_membership(val, 0.25, 0.35, 0.5, 0.6) for val in x]
    good = [trapezoid_membership(val, 0.5, 0.6, 0.75, 0.85) for val in x]
    excellent = [trapezoid_membership(val, 0.75, 0.85, 1.0, 1.0) for val in x]
    
    plt.figure(figsize=(10, 6))
    plt.fill_between(x, 0, poor, alpha=0.3, color='red', label='Poor')
    plt.plot(x, poor, 'r-', linewidth=2.5)
    plt.fill_between(x, 0, fair, alpha=0.3, color='orange', label='Fair')
    plt.plot(x, fair, color='orange', linewidth=2.5)
    plt.fill_between(x, 0, good, alpha=0.3, color='yellowgreen', label='Good')
    plt.plot(x, good, color='yellowgreen', linewidth=2.5)
    plt.fill_between(x, 0, excellent, alpha=0.3, color='darkgreen', label='Excellent')
    plt.plot(x, excellent, color='darkgreen', linewidth=2.5)
    
    plt.xlabel('Fuzzy Score (0-1)', fontsize=12, fontweight='bold')
    plt.ylabel('Membership Degree', fontsize=12, fontweight='bold')
    plt.title('Fuzzy Score Output Membership Functions', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11, loc='upper right')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.xlim(0, 1)
    plt.ylim(-0.05, 1.1)
    plt.tight_layout()
    
    return plt.gcf()





def plot_time_fuzzification_example():
    """Plot example of fuzzification process for Travel Time = 520 seconds"""
    x = np.linspace(250, 950, 1000)
    
    # Define membership functions
    short = [trapezoid_membership(val, 300, 300, 400, 450) for val in x]
    medium = [trapezoid_membership(val, 400, 450, 600, 650) for val in x]
    long = [trapezoid_membership(val, 600, 650, 900, 900) for val in x]
    
    # Example value
    example_value = 520
    short_degree = trapezoid_membership(example_value, 300, 300, 400, 450)
    medium_degree = trapezoid_membership(example_value, 400, 450, 600, 650)
    long_degree = trapezoid_membership(example_value, 600, 650, 900, 900)
    
    plt.figure(figsize=(10, 6))
    plt.fill_between(x, 0, short, alpha=0.2, color='darkgreen')
    plt.plot(x, short, color='darkgreen', linewidth=2.5, label='Short')
    plt.fill_between(x, 0, medium, alpha=0.2, color='orange')
    plt.plot(x, medium, color='orange', linewidth=2.5, label='Medium')
    plt.fill_between(x, 0, long, alpha=0.2, color='red')
    plt.plot(x, long, 'r-', linewidth=2.5, label='Long')
    
    # Mark the example point
    plt.axvline(x=example_value, color='black', linestyle='--', linewidth=2, 
                label=f'Input: {example_value}s (8.7 min)')
    plt.plot(example_value, short_degree, 'o', color='darkgreen', markersize=12, 
             label=f'Short = {short_degree:.2f}')
    plt.plot(example_value, medium_degree, 'o', color='orange', markersize=12, 
             label=f'Medium = {medium_degree:.2f}')
    plt.plot(example_value, long_degree, 'ro', markersize=12, 
             label=f'Long = {long_degree:.2f}')
    
    plt.xlabel('Travel Time (seconds)', fontsize=12, fontweight='bold')
    plt.ylabel('Membership Degree', fontsize=12, fontweight='bold')
    plt.title('Fuzzification Example: Travel Time = 520 seconds', 
              fontsize=14, fontweight='bold')
    plt.legend(fontsize=10, loc='upper right')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.xlim(250, 950)
    plt.ylim(-0.05, 1.1)
    plt.tight_layout()
    
    return plt.gcf()


def plot_power_fuzzification_example():
    """Plot example of fuzzification process for Motor Power = 625 kW"""
    x = np.linspace(200, 1100, 1000)
    
    # Define membership functions
    low = [trapezoid_membership(val, 300, 300, 500, 550) for val in x]
    medium = [trapezoid_membership(val, 500, 550, 700, 750) for val in x]
    high = [trapezoid_membership(val, 700, 750, 1000, 1000) for val in x]
    
    # Example value
    example_value = 625
    low_degree = trapezoid_membership(example_value, 300, 300, 500, 550)
    medium_degree = trapezoid_membership(example_value, 500, 550, 700, 750)
    high_degree = trapezoid_membership(example_value, 700, 750, 1000, 1000)
    
    plt.figure(figsize=(10, 6))
    plt.fill_between(x, 0, low, alpha=0.2, color='darkgreen')
    plt.plot(x, low, color='darkgreen', linewidth=2.5, label='Low')
    plt.fill_between(x, 0, medium, alpha=0.2, color='orange')
    plt.plot(x, medium, color='orange', linewidth=2.5, label='Medium')
    plt.fill_between(x, 0, high, alpha=0.2, color='red')
    plt.plot(x, high, 'r-', linewidth=2.5, label='High')
    
    # Mark the example point
    plt.axvline(x=example_value, color='black', linestyle='--', linewidth=2, 
                label=f'Input: {example_value} kW')
    plt.plot(example_value, low_degree, 'o', color='darkgreen', markersize=12, 
             label=f'Low = {low_degree:.2f}')
    plt.plot(example_value, medium_degree, 'o', color='orange', markersize=12, 
             label=f'Medium = {medium_degree:.2f}')
    plt.plot(example_value, high_degree, 'ro', markersize=12, 
             label=f'High = {high_degree:.2f}')
    
    plt.xlabel('Motor Power per Car (kW)', fontsize=12, fontweight='bold')
    plt.ylabel('Membership Degree', fontsize=12, fontweight='bold')
    plt.title('Fuzzification Example: Motor Power = 625 kW', 
              fontsize=14, fontweight='bold')
    plt.legend(fontsize=10, loc='upper right')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.xlim(200, 1100)
    plt.ylim(-0.05, 1.1)
    plt.tight_layout()
    
    return plt.gcf()


def plot_all_inputs_combined():
    """Plot main fuzzy inputs: Travel Time and Motor Power + Gradient context"""
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    
    # Travel Time (Main Input)
    x1 = np.linspace(250, 950, 1000)
    short1 = [trapezoid_membership(val, 300, 300, 400, 450) for val in x1]
    med1 = [trapezoid_membership(val, 400, 450, 600, 650) for val in x1]
    long1 = [trapezoid_membership(val, 600, 650, 900, 900) for val in x1]
    axes[0, 0].fill_between(x1, 0, short1, alpha=0.3, color='darkgreen')
    axes[0, 0].plot(x1, short1, color='darkgreen', linewidth=2, label='Short')
    axes[0, 0].fill_between(x1, 0, med1, alpha=0.3, color='orange')
    axes[0, 0].plot(x1, med1, color='orange', linewidth=2, label='Medium')
    axes[0, 0].fill_between(x1, 0, long1, alpha=0.3, color='red')
    axes[0, 0].plot(x1, long1, 'r-', linewidth=2, label='Long')
    axes[0, 0].set_xlabel('Travel Time (seconds)', fontweight='bold')
    axes[0, 0].set_ylabel('Membership', fontweight='bold')
    axes[0, 0].set_title('(a) Travel Time - Optimization Goal', fontweight='bold')
    axes[0, 0].legend(fontsize=9)
    axes[0, 0].grid(True, alpha=0.3, linestyle='--')
    axes[0, 0].set_ylim(-0.05, 1.1)
    
    # Motor Power per Car (Main Input)
    x2 = np.linspace(200, 1100, 1000)
    low2 = [trapezoid_membership(val, 300, 300, 500, 550) for val in x2]
    med2 = [trapezoid_membership(val, 500, 550, 700, 750) for val in x2]
    high2 = [trapezoid_membership(val, 700, 750, 1000, 1000) for val in x2]
    axes[0, 1].fill_between(x2, 0, low2, alpha=0.3, color='darkgreen')
    axes[0, 1].plot(x2, low2, color='darkgreen', linewidth=2, label='Low')
    axes[0, 1].fill_between(x2, 0, med2, alpha=0.3, color='orange')
    axes[0, 1].plot(x2, med2, color='orange', linewidth=2, label='Medium')
    axes[0, 1].fill_between(x2, 0, high2, alpha=0.3, color='red')
    axes[0, 1].plot(x2, high2, 'r-', linewidth=2, label='High')
    axes[0, 1].set_xlabel('Motor Power per Car (kW)', fontweight='bold')
    axes[0, 1].set_ylabel('Membership', fontweight='bold')
    axes[0, 1].set_title('(b) Motor Power - Optimization Goal', fontweight='bold')
    axes[0, 1].legend(fontsize=9)
    axes[0, 1].grid(True, alpha=0.3, linestyle='--')
    axes[0, 1].set_ylim(-0.05, 1.1)
    
    # Gradient (Context/Constraint)
    x3 = np.linspace(-5, 30, 1000)
    low3 = [trapezoid_membership(val, 0, 0, 5, 8) for val in x3]
    med3 = [trapezoid_membership(val, 5, 8, 12, 15) for val in x3]
    high3 = [trapezoid_membership(val, 12, 15, 25, 25) for val in x3]
    axes[1, 0].fill_between(x3, 0, low3, alpha=0.3, color='blue')
    axes[1, 0].plot(x3, low3, 'b-', linewidth=2, label='Low')
    axes[1, 0].fill_between(x3, 0, med3, alpha=0.3, color='green')
    axes[1, 0].plot(x3, med3, 'g-', linewidth=2, label='Medium')
    axes[1, 0].fill_between(x3, 0, high3, alpha=0.3, color='red')
    axes[1, 0].plot(x3, high3, 'r-', linewidth=2, label='High')
    axes[1, 0].set_xlabel('Gradient (‰ per-mill)', fontweight='bold')
    axes[1, 0].set_ylabel('Membership', fontweight='bold')
    axes[1, 0].set_title('(c) Gradient - Route Context', fontweight='bold')
    axes[1, 0].legend(fontsize=9)
    axes[1, 0].grid(True, alpha=0.3, linestyle='--')
    axes[1, 0].axvline(x=0, color='k', linestyle=':', alpha=0.5)
    axes[1, 0].set_ylim(-0.05, 1.1)
    
    # Fuzzy Score Output
    x4 = np.linspace(0, 1, 1000)
    poor4 = [trapezoid_membership(val, 0, 0, 0.25, 0.35) for val in x4]
    fair4 = [trapezoid_membership(val, 0.25, 0.35, 0.5, 0.6) for val in x4]
    good4 = [trapezoid_membership(val, 0.5, 0.6, 0.75, 0.85) for val in x4]
    exc4 = [trapezoid_membership(val, 0.75, 0.85, 1.0, 1.0) for val in x4]
    axes[1, 1].fill_between(x4, 0, poor4, alpha=0.3, color='red')
    axes[1, 1].plot(x4, poor4, 'r-', linewidth=2, label='Poor')
    axes[1, 1].fill_between(x4, 0, fair4, alpha=0.3, color='orange')
    axes[1, 1].plot(x4, fair4, color='orange', linewidth=2, label='Fair')
    axes[1, 1].fill_between(x4, 0, good4, alpha=0.3, color='yellowgreen')
    axes[1, 1].plot(x4, good4, color='yellowgreen', linewidth=2, label='Good')
    axes[1, 1].fill_between(x4, 0, exc4, alpha=0.3, color='darkgreen')
    axes[1, 1].plot(x4, exc4, color='darkgreen', linewidth=2, label='Excellent')
    axes[1, 1].set_xlabel('Fuzzy Score (0-1)', fontweight='bold')
    axes[1, 1].set_ylabel('Membership', fontweight='bold')
    axes[1, 1].set_title('(d) Fuzzy Score - Output', fontweight='bold')
    axes[1, 1].legend(fontsize=8)
    axes[1, 1].grid(True, alpha=0.3, linestyle='--')
    axes[1, 1].set_ylim(-0.05, 1.1)
    
    plt.tight_layout()
    return fig


def main(display_inline=True, save_files=True):
    """
    Generate all membership function graphs
    
    Parameters:
    - display_inline: Show plots in notebook (True for Colab)
    - save_files: Save PNG/PDF files (True for downloading from Colab)
    """
    # Create output directory
    if save_files:
        # For Colab, save to current directory
        output_dir = 'fuzzy_graphs'
        os.makedirs(output_dir, exist_ok=True)
        print("Generating fuzzy membership function graphs...")
        print(f"Output directory: {output_dir}")
    else:
        print("Generating fuzzy membership function graphs (display only)...")
    
    # Generate individual graphs
    plots = [
        ("travel_time", plot_travel_time_membership, "Travel Time Membership Functions"),
        ("motor_power_per_car", plot_motor_power_membership, "Motor Power per Car Membership Functions"),
        ("gradient", plot_gradient_membership, "Gradient Membership Functions"),
        ("acceleration", plot_acceleration_membership, "Acceleration Membership Functions (Design Parameter)"),
        ("fuzzy_score_output", plot_fuzzy_score_output, "Fuzzy Score Output Membership Functions"),
        ("time_fuzzification_example", plot_time_fuzzification_example, "Travel Time Fuzzification Example"),
        ("power_fuzzification_example", plot_power_fuzzification_example, "Motor Power Fuzzification Example"),
        ("all_inputs_combined", plot_all_inputs_combined, "All Fuzzy System Components"),
    ]
    
    for name, plot_func, description in plots:
        print(f"\n{'='*60}")
        print(f"  {description}")
        print(f"{'='*60}")
        
        fig = plot_func()
        
        if save_files:
            # Save as PNG (high resolution)
            png_path = os.path.join(output_dir, f'{name}.png')
            fig.savefig(png_path, dpi=300, bbox_inches='tight')
            
            # Save as PDF (vector format for publications)
            pdf_path = os.path.join(output_dir, f'{name}.pdf')
            fig.savefig(pdf_path, format='pdf', bbox_inches='tight')
            print(f"  ✓ Saved: {name}.png and {name}.pdf")
        
        if display_inline:
            # Display in Colab/Jupyter notebook
            plt.show()
        else:
            plt.close(fig)
    
    if save_files:
        print("\n" + "="*60)
        print("✓ All graphs generated successfully!")
        print(f"\nImages saved to: {output_dir}/")
        print("\nGenerated files:")
        print("  - travel_time.png/pdf")
        print("  - motor_power_per_car.png/pdf")
        print("  - gradient.png/pdf")
        print("  - acceleration.png/pdf")
        print("  - fuzzy_score_output.png/pdf")
        print("  - time_fuzzification_example.png/pdf")
        print("  - power_fuzzification_example.png/pdf")
        print("  - all_inputs_combined.png/pdf")
        print("\n📥 In Google Colab, you can download files from the left sidebar")
        print("   (Files icon → fuzzy_graphs folder → right-click → Download)")
    
    print("\n✓ You can use these images in your thesis presentation!")


if __name__ == "__main__":
    # When run locally, save files but don't display inline
    main(display_inline=False, save_files=True)
