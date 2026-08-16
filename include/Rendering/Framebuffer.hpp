#pragma once

namespace nsim::rendering {

/**
 * Offscreen framebuffer for HDR rendering and post-processing.
 */
class Framebuffer {
public:
    Framebuffer() = default;
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    /**
     * Create framebuffer with color texture.
     * @param width Width in pixels
     * @param height Height in pixels
     */
    void create(int width, int height);

    /**
     * Resize the framebuffer.
     */
    void resize(int width, int height);

    /**
     * Bind for rendering.
     */
    void bind() const;

    /**
     * Unbind (bind default framebuffer).
     */
    void unbind() const;

    /**
     * Color texture ID.
     */
    unsigned int colorTexture() const { return colorTex_; }

    /**
     * Framebuffer object ID.
     */
    unsigned int id() const { return fbo_; }

    /**
     * Framebuffer width in pixels.
     */
    int width() const { return width_; }

    /**
     * Framebuffer height in pixels.
     */
    int height() const { return height_; }

    /**
     * Free GPU resources.
     */
    void destroy();

private:
    unsigned int fbo_ = 0;
    unsigned int colorTex_ = 0;
    unsigned int rbo_ = 0;
    int width_ = 0;
    int height_ = 0;
};

} // namespace nsim::rendering