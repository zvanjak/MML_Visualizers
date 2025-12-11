#!/usr/bin/env python3
"""
Quick visualization script for MML data files
Demonstrates the three new FLTK visualizers' data
"""

import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.patches import Circle, FancyArrow
import numpy as np

def parse_parametric_curve(filename):
    """Parse PARAMETRIC_CURVE_CARTESIAN_2D format"""
    with open(filename, 'r') as f:
        lines = [l.strip() for l in f.readlines() if l.strip()]
    
    title = lines[1]
    data = []
    for line in lines[5:]:  # Skip header lines
        parts = line.split()
        if len(parts) >= 3:
            t, x, y = float(parts[0]), float(parts[1]), float(parts[2])
            data.append((t, x, y))
    
    return title, np.array(data)

def parse_particle_simulation(filename):
    """Parse PARTICLE_SIMULATION_DATA_2D format"""
    with open(filename, 'r') as f:
        lines = [l.strip() for l in f.readlines() if l.strip()]
    
    # Parse header
    num_balls = int(lines[1].split()[1])
    balls = []
    for i in range(num_balls):
        parts = lines[2 + i].split()
        balls.append({
            'name': parts[0],
            'color': parts[1].lower(),
            'radius': float(parts[2]),
            'positions': []
        })
    
    num_steps = int(lines[2 + num_balls].split()[1])
    
    # Parse steps
    line_idx = 3 + num_balls
    for step in range(num_steps):
        line_idx += 1  # Skip "Step X time" line
        for ball_idx in range(num_balls):
            parts = lines[line_idx].split()
            x, y = float(parts[1]), float(parts[2])
            balls[ball_idx]['positions'].append((x, y))
            line_idx += 1
    
    return balls, num_steps

def parse_vector_field(filename):
    """Parse VECTOR_FIELD_2D_CARTESIAN format"""
    with open(filename, 'r') as f:
        lines = [l.strip() for l in f.readlines() if l.strip()]
    
    title = lines[1]
    vectors = []
    for line in lines[2:]:
        parts = line.split()
        if len(parts) >= 4:
            px, py, vx, vy = float(parts[0]), float(parts[1]), float(parts[2]), float(parts[3])
            vectors.append((px, py, vx, vy))
    
    return title, np.array(vectors)

def visualize_parametric_curves():
    """Visualize parametric curves"""
    fig, axes = plt.subplots(2, 2, figsize=(14, 12))
    fig.suptitle('MML Parametric Curve 2D Visualizer - Demo', fontsize=16, fontweight='bold')
    
    curves = [
        '../WPF/MML_ParametricCurve2D_Visualizer/data/curve1.txt',
        '../WPF/MML_ParametricCurve2D_Visualizer/data/curve2.txt',
        '../WPF/MML_ParametricCurve2D_Visualizer/data/curve3.txt',
        '../WPF/MML_ParametricCurve2D_Visualizer/data/curve4.txt'
    ]
    
    colors = ['blue', 'red', 'green', 'orange']
    
    for idx, (curve_file, color) in enumerate(zip(curves, colors)):
        try:
            ax = axes[idx // 2, idx % 2]
            title, data = parse_parametric_curve(curve_file)
            
            ax.plot(data[:, 1], data[:, 2], color=color, linewidth=2, label=title)
            ax.set_title(title, fontweight='bold')
            ax.set_xlabel('X')
            ax.set_ylabel('Y')
            ax.grid(True, alpha=0.3)
            ax.set_aspect('equal', adjustable='box')
            ax.legend()
            
            # Add start and end markers
            ax.plot(data[0, 1], data[0, 2], 'go', markersize=8, label='Start')
            ax.plot(data[-1, 1], data[-1, 2], 'ro', markersize=8, label='End')
            
        except Exception as e:
            ax.text(0.5, 0.5, f'Could not load\n{curve_file}', 
                   ha='center', va='center', transform=ax.transAxes)
    
    plt.tight_layout()
    plt.savefig('parametric_curves_demo.png', dpi=150, bbox_inches='tight')
    print("✓ Saved: parametric_curves_demo.png")
    plt.show()

def visualize_particle_simulation():
    """Visualize particle simulation with animation"""
    try:
        balls, num_steps = parse_particle_simulation(
            '../WPF/MML_ParticleVisualizer2D/data/SimData.txt'
        )
        
        fig, ax = plt.subplots(figsize=(12, 10))
        fig.suptitle('MML Particle Visualizer 2D - Demo', fontsize=16, fontweight='bold')
        
        # Get bounds
        all_x = [pos[0] for ball in balls for pos in ball['positions']]
        all_y = [pos[1] for ball in balls for pos in ball['positions']]
        x_min, x_max = min(all_x), max(all_x)
        y_min, y_max = min(all_y), max(all_y)
        
        # Add padding
        x_padding = (x_max - x_min) * 0.1
        y_padding = (y_max - y_min) * 0.1
        
        ax.set_xlim(x_min - x_padding, x_max + x_padding)
        ax.set_ylim(y_min - y_padding, y_max + y_padding)
        ax.set_aspect('equal')
        ax.grid(True, alpha=0.3)
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        
        # Create circles for each ball
        circles = []
        for ball in balls:
            circle = Circle((0, 0), ball['radius'], 
                          color=ball['color'], alpha=0.7, 
                          label=ball['name'])
            ax.add_patch(circle)
            circles.append(circle)
        
        ax.legend(loc='upper right')
        
        step_text = ax.text(0.02, 0.98, '', transform=ax.transAxes, 
                           verticalalignment='top', fontsize=12, 
                           bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
        
        def animate(frame):
            for circle, ball in zip(circles, balls):
                if frame < len(ball['positions']):
                    pos = ball['positions'][frame]
                    circle.center = pos
            step_text.set_text(f'Step: {frame} / {num_steps-1}\nTime: {frame * 0.1:.1f}')
            return circles + [step_text]
        
        anim = animation.FuncAnimation(fig, animate, frames=num_steps, 
                                      interval=200, blit=True, repeat=True)
        
        plt.tight_layout()
        plt.savefig('particle_simulation_demo.png', dpi=150, bbox_inches='tight')
        print("✓ Saved: particle_simulation_demo.png")
        plt.show()
        
    except Exception as e:
        print(f"Error visualizing particle simulation: {e}")

def visualize_vector_field():
    """Visualize vector field"""
    try:
        title, vectors = parse_vector_field(
            '../WPF/MML_VectorField2D_Visualizer/data/example3_wpf_vector_field_2d.txt'
        )
        
        fig, ax = plt.subplots(figsize=(12, 10))
        fig.suptitle(f'MML Vector Field 2D Visualizer - {title}', 
                    fontsize=16, fontweight='bold')
        
        # Extract data
        px, py, vx, vy = vectors[:, 0], vectors[:, 1], vectors[:, 2], vectors[:, 3]
        
        # Calculate magnitudes for color mapping
        magnitudes = np.sqrt(vx**2 + vy**2)
        
        # Create quiver plot
        quiver = ax.quiver(px, py, vx, vy, magnitudes, 
                          cmap='viridis', alpha=0.8,
                          scale=50, width=0.003)
        
        # Add colorbar
        cbar = plt.colorbar(quiver, ax=ax)
        cbar.set_label('Vector Magnitude', rotation=270, labelpad=20)
        
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.set_aspect('equal')
        ax.grid(True, alpha=0.3)
        
        # Add statistics
        stats_text = f'Vectors: {len(vectors)}\nMax magnitude: {magnitudes.max():.2f}'
        ax.text(0.02, 0.98, stats_text, transform=ax.transAxes,
               verticalalignment='top', fontsize=10,
               bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
        
        plt.tight_layout()
        plt.savefig('vector_field_demo.png', dpi=150, bbox_inches='tight')
        print("✓ Saved: vector_field_demo.png")
        plt.show()
        
    except Exception as e:
        print(f"Error visualizing vector field: {e}")

if __name__ == '__main__':
    print("=" * 60)
    print("MML Visualizers Demo")
    print("=" * 60)
    print()
    
    print("1. Visualizing Parametric Curves...")
    visualize_parametric_curves()
    print()
    
    print("2. Visualizing Particle Simulation...")
    visualize_particle_simulation()
    print()
    
    print("3. Visualizing Vector Field...")
    visualize_vector_field()
    print()
    
    print("=" * 60)
    print("Demo Complete!")
    print("Check the generated PNG files for static visualization.")
    print("=" * 60)
