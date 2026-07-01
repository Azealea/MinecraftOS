{
  description = "Wayland Vulkan compositor starter";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = {
    self,
    nixpkgs,
  }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {inherit system;};
  in {
    devShells.${system}.default = pkgs.mkShell {
      buildInputs = [
        pkgs.wayland
        pkgs.wayland-protocols
        pkgs.wayland-scanner
        pkgs.wlroots_0_19
        pkgs.libinput
        pkgs.pixman
        pkgs.libxkbcommon
        pkgs.libdrm

        # Vulkan runtime + build + tools
        pkgs.vulkan-loader # loader/runtime
        pkgs.vulkan-headers # headers for building
        pkgs.vulkan-tools # includes `vulkaninfo`
        pkgs.vulkan-validation-layers # VK_LAYER_KHRONOS_validation
        pkgs.shaderc

        # vulkan testing purpose :
        pkgs.glfw

        # drivers
        pkgs.mesa

        pkgs.gcc
        pkgs.ccache
        pkgs.pkg-config

        # math helper
        pkgs.cglm
      ];

      shellHook = ''
        # intel gpu
        export VK_ICD_FILENAMES=${pkgs.mesa}/share/vulkan/icd.d/intel_icd.x86_64.json

        # AMD bros
        # export VK_ICD_FILENAMES=${pkgs.mesa}/share/vulkan/icd.d/radeon_icd.x86_64.json

        # Vulkan validation layers
        export VK_LAYER_PATH=${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d

        # idk man..
        export VK_INSTANCE_LAYERS=VK_LAYER_KHRONOS_validation

        echo "ICD: $VK_ICD_FILENAMES"
        echo "VK_LAYER_PATH: $VK_LAYER_PATH"

      '';
    };
  };
}
